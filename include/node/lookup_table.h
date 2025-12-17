#ifndef LOOKUP_TABLE_H
#define LOOKUP_TABLE_H

#include <stddef.h>

#define HASH_SIZE 20  // SHA-1 hash size
#define MAX_HOST_LEN 46  // IPv6 max length (45) + null terminator

// Node structure
typedef struct {
    char host[MAX_HOST_LEN];
    int port;
    char hash[HASH_SIZE];
} node_info_t;

// Lookup table handle (opaque type for encapsulation)
typedef struct lookup_table_t lookup_table_t;

// Create and destroy lookup table
lookup_table_t* lookup_table_create(void);
void lookup_table_destroy(lookup_table_t* table);

// Thread-safe operations
int lookup_table_add(lookup_table_t* table, const char* host, int port, const char* hash);
int lookup_table_remove(lookup_table_t* table, const char* host, int port);
int lookup_table_remove_by_hash(lookup_table_t* table, const char* hash);
node_info_t* lookup_table_find(lookup_table_t* table, const char* host, int port);
node_info_t* lookup_table_find_by_hash(lookup_table_t* table, const char* hash);

// Serialization/deserialization
size_t lookup_table_serialized_size(lookup_table_t* table);
int lookup_table_serialize(lookup_table_t* table, char* buffer, size_t buffer_size);
lookup_table_t* lookup_table_deserialize(const char* data, size_t data_size);

// Utility functions
size_t lookup_table_size(lookup_table_t* table);
void lookup_table_print(lookup_table_t* table);

#endif // LOOKUP_TABLE_H