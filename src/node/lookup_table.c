#include "node/lookup_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Internal node structure for linked list
typedef struct TableNode {
    node_info_t info;
    struct TableNode* next;
} TableNode;

// Lookup table structure
struct lookup_table_t {
    TableNode* head;
    pthread_mutex_t mutex;
    size_t size;
};

// Helper function to create a new node
static TableNode* create_node(const char* host, int port, const char* hash) {
    TableNode* node = (TableNode*)malloc(sizeof(TableNode));
    if (!node) return NULL;
    
    strncpy(node->info.host, host, MAX_HOST_LEN - 1);
    node->info.host[MAX_HOST_LEN - 1] = '\0';
    node->info.port = port;
    memcpy(node->info.hash, hash, HASH_SIZE);
    node->next = NULL;
    
    return node;
}

// Helper function to compare node info
// static int node_info_equal(const node_info_t* a, const node_info_t* b) {
//     return (strcmp(a->host, b->host) == 0) && (a->port == b->port);
// }

// Helper function to compare hashes
static int hash_equal(const char* hash1, const char* hash2) {
    return memcmp(hash1, hash2, HASH_SIZE) == 0;
}

lookup_table_t* lookup_table_create(void) {
    lookup_table_t* table = (lookup_table_t*)malloc(sizeof(lookup_table_t));
    if (!table) return NULL;
    
    table->head = NULL;
    table->size = 0;
    
    if (pthread_mutex_init(&table->mutex, NULL) != 0) {
        free(table);
        return NULL;
    }
    
    return table;
}

void lookup_table_destroy(lookup_table_t* table) {
    if (!table) return;
    
    pthread_mutex_lock(&table->mutex);
    
    TableNode* current = table->head;
    while (current) {
        TableNode* next = current->next;
        free(current);
        current = next;
    }
    
    pthread_mutex_unlock(&table->mutex);
    pthread_mutex_destroy(&table->mutex);
    free(table);
}

int lookup_table_add(lookup_table_t* table, const char* host, int port, const char* hash) {
    if (!table || !host || !hash) return -1;
    
    pthread_mutex_lock(&table->mutex);
    
    // Check if node already exists
    TableNode* current = table->head;
    while (current) {
        if (strcmp(current->info.host, host) == 0 && current->info.port == port) {
            pthread_mutex_unlock(&table->mutex);
            return 0;  // Already exists
        }
        current = current->next;
    }
    
    // Create and add new node
    TableNode* new_node = create_node(host, port, hash);
    if (!new_node) {
        pthread_mutex_unlock(&table->mutex);
        return -1;
    }
    
    new_node->next = table->head;
    table->head = new_node;
    table->size++;
    
    pthread_mutex_unlock(&table->mutex);
    return 1;
}

int lookup_table_remove(lookup_table_t* table, const char* host, int port) {
    if (!table || !host) return -1;
    
    pthread_mutex_lock(&table->mutex);
    
    TableNode* current = table->head;
    TableNode* prev = NULL;
    int removed = 0;
    
    while (current) {
        if (strcmp(current->info.host, host) == 0 && current->info.port == port) {
            if (prev) {
                prev->next = current->next;
            } else {
                table->head = current->next;
            }
            
            free(current);
            table->size--;
            removed = 1;
            break;
        }
        prev = current;
        current = current->next;
    }
    
    pthread_mutex_unlock(&table->mutex);
    return removed;
}

int lookup_table_remove_by_hash(lookup_table_t* table, const char* hash) {
    if (!table || !hash) return -1;
    
    pthread_mutex_lock(&table->mutex);
    
    TableNode* current = table->head;
    TableNode* prev = NULL;
    int removed = 0;
    
    while (current) {
        if (hash_equal(current->info.hash, hash)) {
            if (prev) {
                prev->next = current->next;
            } else {
                table->head = current->next;
            }
            
            free(current);
            table->size--;
            removed = 1;
            break;
        }
        prev = current;
        current = current->next;
    }
    
    pthread_mutex_unlock(&table->mutex);
    return removed;
}

node_info_t* lookup_table_find(lookup_table_t* table, const char* host, int port) {
    if (!table || !host) return NULL;
    
    pthread_mutex_lock(&table->mutex);
    
    TableNode* current = table->head;
    node_info_t* result = NULL;
    
    while (current) {
        if (strcmp(current->info.host, host) == 0 && current->info.port == port) {
            result = &current->info;
            break;
        }
        current = current->next;
    }
    
    pthread_mutex_unlock(&table->mutex);
    return result;
}

node_info_t* lookup_table_find_by_hash(lookup_table_t* table, const char* hash) {
    if (!table || !hash) return NULL;
    
    pthread_mutex_lock(&table->mutex);
    
    TableNode* current = table->head;
    node_info_t* result = NULL;
    
    while (current) {
        if (hash_equal(current->info.hash, hash)) {
            result = &current->info;
            break;
        }
        current = current->next;
    }
    
    pthread_mutex_unlock(&table->mutex);
    return result;
}

size_t lookup_table_serialized_size(lookup_table_t* table) {
    if (!table) return 0;
    
    pthread_mutex_lock(&table->mutex);
    size_t size = sizeof(size_t);  // For storing number of nodes
    size += table->size * (MAX_HOST_LEN + sizeof(int) + HASH_SIZE);
    pthread_mutex_unlock(&table->mutex);
    
    return size;
}

int lookup_table_serialize(lookup_table_t* table, char* buffer, size_t buffer_size) {
    if (!table || !buffer) return -1;
    
    pthread_mutex_lock(&table->mutex);
    
    size_t required_size = lookup_table_serialized_size(table);
    if (buffer_size < required_size) {
        pthread_mutex_unlock(&table->mutex);
        return -1;
    }
    
    // Write number of nodes
    memcpy(buffer, &table->size, sizeof(size_t));
    size_t offset = sizeof(size_t);
    
    // Write each node
    TableNode* current = table->head;
    while (current) {
        // Write host
        memcpy(buffer + offset, current->info.host, MAX_HOST_LEN);
        offset += MAX_HOST_LEN;
        
        // Write port
        memcpy(buffer + offset, &current->info.port, sizeof(int));
        offset += sizeof(int);
        
        // Write hash
        memcpy(buffer + offset, current->info.hash, HASH_SIZE);
        offset += HASH_SIZE;
        
        current = current->next;
    }
    
    pthread_mutex_unlock(&table->mutex);
    return 0;
}

lookup_table_t* lookup_table_deserialize(const char* data, size_t data_size) {
    if (!data || data_size < sizeof(size_t)) return NULL;
    
    lookup_table_t* table = lookup_table_create();
    if (!table) return NULL;
    
    // Read number of nodes
    size_t node_count;
    memcpy(&node_count, data, sizeof(size_t));
    size_t offset = sizeof(size_t);
    
    // Calculate expected size
    size_t expected_size = sizeof(size_t) + 
                          node_count * (MAX_HOST_LEN + sizeof(int) + HASH_SIZE);
    
    if (data_size < expected_size) {
        lookup_table_destroy(table);
        return NULL;
    }
    
    pthread_mutex_lock(&table->mutex);
    
    // Read each node
    for (size_t i = 0; i < node_count; i++) {
        // Read host
        char host[MAX_HOST_LEN];
        memcpy(host, data + offset, MAX_HOST_LEN);
        offset += MAX_HOST_LEN;
        
        // Read port
        int port;
        memcpy(&port, data + offset, sizeof(int));
        offset += sizeof(int);
        
        // Read hash
        char hash[HASH_SIZE];
        memcpy(hash, data + offset, HASH_SIZE);
        offset += HASH_SIZE;
        
        // Add node to table
        TableNode* new_node = create_node(host, port, hash);
        if (!new_node) {
            pthread_mutex_unlock(&table->mutex);
            lookup_table_destroy(table);
            return NULL;
        }
        
        new_node->next = table->head;
        table->head = new_node;
        table->size++;
    }
    
    pthread_mutex_unlock(&table->mutex);
    return table;
}

size_t lookup_table_size(lookup_table_t* table) {
    if (!table) return 0;
    
    pthread_mutex_lock(&table->mutex);
    size_t size = table->size;
    pthread_mutex_unlock(&table->mutex);
    
    return size;
}

void lookup_table_print(lookup_table_t* table) {
    if (!table) return;
    
    pthread_mutex_lock(&table->mutex);
    
    printf("Lookup Table (%zu nodes):\n", table->size);
    printf("---------------------------\n");
    
    TableNode* current = table->head;
    int index = 1;
    while (current) {
        printf("Node %d:\n", index++);
        printf("  Host: %s\n", current->info.host);
        printf("  Port: %d\n", current->info.port);
        printf("  Hash: ");
        for (int i = 0; i < HASH_SIZE; i++) {
            printf("%02x", (unsigned char)current->info.hash[i]);
        }
        printf("\n");
        current = current->next;
    }
    
    pthread_mutex_unlock(&table->mutex);
}