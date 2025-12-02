#include "helpers/xqueue.h"

#include <malloc.h>

void xqueue_new(xqueue_t **queue) {
    (*queue) = malloc(sizeof(xqueue_t));
    (*queue)->head = NULL;
    (*queue)->tail = NULL;
    pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;
    (*queue)->lock = my_mutex;
}

void xqueue_enqueue(xqueue_t *queue, void *data) {
    pthread_mutex_lock(&queue->lock);
    xquque_node_t *last_head = queue->head;
    xquque_node_t *node = malloc(sizeof(xquque_node_t));
    node->next = NULL;
    node->data = data;
    queue->head = node;
    if(last_head != NULL) {
        last_head->next = node;
    }
    if(queue->tail == NULL) {
        queue->tail = node;
    }
    pthread_mutex_unlock(&queue->lock);
}

void xqueue_dequeue(xqueue_t *queue, void **output) {
    pthread_mutex_lock(&queue->lock);
    if(queue->tail == NULL) {
        (*output) = NULL;
        return;
    }
    xquque_node_t *tail = queue->tail;
    (*output) = tail->data;
    queue->tail = tail->next;
    free(tail);
    pthread_mutex_unlock(&queue->lock);
}

void xqueue_destroy(xqueue_t *queue) {
    while (1){
        void *out;
        xqueue_dequeue(queue, &out);
        if(out == NULL) {
            break;
        }
        free(out);
    }
    pthread_mutex_destroy(&queue->lock);
    free(queue);
}