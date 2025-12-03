#ifndef DATABASE_BUCKET_H
#define DATABASE_BUCKET_H

#include <pthread.h>
#include <stddef.h>

typedef struct {
    char key[20];
} kv_key_t;

struct kv_entry_t;
struct kv_entry_t {
    kv_key_t key;
    size_t data_length;
    char *data;
    struct kv_entry_t *next;
};
typedef struct kv_entry_t kv_entry_t;

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

#endif // DATABASE_BUCKET_H
