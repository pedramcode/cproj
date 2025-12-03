#include "database/store.h"

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "helpers/sha1.h"

size_t sha1_mod_bucket(const char hash[20], size_t bucket_count) {
    size_t result = 0;
    for (int i = 0; i < 20; i++) {
        result = (result * 256 + hash[i]) % bucket_count;
    }
    return result;
}

void kv_store_new(kv_store_t **store, size_t bucket_count) {
    (*store) = malloc(sizeof(kv_store_t));
    kv_store_t *s = (*store);
    s->bucket_count = bucket_count;
    s->buckets = (kv_entry_t**) calloc(s->bucket_count, sizeof(kv_entry_t*));
    s->bucket_locks = malloc(sizeof(pthread_mutex_t) * bucket_count);
    for(size_t i = 0 ; i < bucket_count ; i++){
        pthread_mutex_init(&s->bucket_locks[i], NULL);
    }
}

void kv_entry_new(kv_entry_t **entry, char *key, char* data, size_t data_length, kv_entry_t* next) {
    kv_entry_t *output = malloc(sizeof(kv_entry_t));
    output->data = malloc(data_length);
    memcpy(output->data, data, data_length);
    output->data_length = data_length;
    memcpy(&output->key, key, 20);
    output->next = next;
    (*entry) = output;
}

void kv_store_put(kv_store_t *store, char *key, size_t key_size, char *data, size_t data_length) {
    char result[20];
    SHA1(result, key, key_size);
    size_t bucket_index = sha1_mod_bucket(result, store->bucket_count);
    
    pthread_mutex_lock(&store->bucket_locks[bucket_index]);
    if(store->buckets[bucket_index] == NULL) {
        printf("FIRST TIME\n");
        kv_entry_new(&store->buckets[bucket_index], result, data, data_length, NULL);
    } else {
        printf("NEXT TIME\n");
        kv_entry_t *node = store->buckets[bucket_index];
        while(1) {
            if(memcmp(result, node->key.key, 20) == 0) {
                // Duplicated key
                break;
            };
            if(node->next == NULL) {
                kv_entry_t *entry;
                kv_entry_new(&entry, result, data, data_length, NULL);
                node->next = entry;
                break;
            }
            node = node->next;
        }
    }
    pthread_mutex_unlock(&store->bucket_locks[bucket_index]);
}

size_t kv_store_get(kv_store_t *store, char *key, size_t key_size, char **output) {
    char result[20];
    SHA1(result, key, key_size);
    size_t bucket_index = sha1_mod_bucket(result, store->bucket_count);

    pthread_mutex_lock(&store->bucket_locks[bucket_index]);
    kv_entry_t *ent = store->buckets[bucket_index];
    if(ent == NULL) {
        (*output) = NULL;
        pthread_mutex_unlock(&store->bucket_locks[bucket_index]);
        return 0;
    }
    while(1) {
        if(memcmp(ent->key.key, result, 20) == 0) {
            (*output) = malloc(ent->data_length);
            memcpy((*output), ent->data, ent->data_length);
            pthread_mutex_unlock(&store->bucket_locks[bucket_index]);
            return ent->data_length;
        }
        if(ent->next == NULL) break;
        ent = ent->next;
    }
    (*output) = NULL;
    pthread_mutex_unlock(&store->bucket_locks[bucket_index]);
    return 0;
}

void kv_entry_destroy(kv_entry_t *entry) {
    free(entry->data);
    free(entry);
}