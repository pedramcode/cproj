#ifndef HELPERS_XQUEUE_H
#define HELPERS_XQUEUE_H

#include <pthread.h>

struct xquque_node_t;
struct xquque_node_t {
    struct xquque_node_t *next;
    void *data;
};
typedef struct xquque_node_t xquque_node_t;

typedef struct {
    xquque_node_t *head;
    xquque_node_t *tail;
    pthread_mutex_t lock;
} xqueue_t;

void xqueue_new(xqueue_t **queue);
void xqueue_enqueue(xqueue_t *queue, void *data);
void xqueue_dequeue(xqueue_t *queue, void **output);
void xqueue_destroy(xqueue_t *queue);

#endif // HELPERS_XQUEUE_H