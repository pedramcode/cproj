#ifndef DATABASE_BUCKET_H
#define DATABASE_BUCKET_H

#include <pthread.h>
#include <stddef.h>

/// @brief key of record in hash table
typedef struct {
    char key[20];
} kv_key_t;

struct kv_entry_t;
/// @brief a single entry of hash table in key:value format
struct kv_entry_t {
    kv_key_t key;
    size_t data_length;
    char *data;
    struct kv_entry_t *next;
};
typedef struct kv_entry_t kv_entry_t;

/// @brief main storage structure, which stores hash table data in "buckets" or batches. each bucket has it's own lock.
typedef struct {
    kv_entry_t **buckets;
    pthread_mutex_t *bucket_locks;
    size_t bucket_count;
} kv_store_t;

void kv_entry_new(kv_entry_t **entry, char *key, char* data, size_t data_length, kv_entry_t* next);
void kv_entry_destroy(kv_entry_t *entry);
void kv_store_new(kv_store_t **store, size_t bucket_count);
void kv_store_put(kv_store_t *store, char *key, size_t key_size, char *data, size_t data_length);
size_t kv_store_get(kv_store_t *store, char *key, size_t key_size, char **output);
void kv_store_destroy(kv_store_t *store);

#endif // DATABASE_BUCKET_H
