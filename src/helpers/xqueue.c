#include "helpers/xqueue.h"

#include <malloc.h>

void xqueue_new(xqueue_t **queue) {
    (*queue) = malloc(sizeof(xqueue_t));
    (*queue)->head = NULL;
    (*queue)->tail = NULL;
    pthread_mutex_init(&(*queue)->lock, NULL);
    pthread_cond_init(&(*queue)->cond, NULL);
}

void xqueue_enqueue(xqueue_t *queue, void *data) {
    xquque_node_t *last_head = queue->head;
    xquque_node_t *node = malloc(sizeof(xquque_node_t));
    node->data = data;
    node->next = NULL;

    pthread_mutex_lock(&queue->lock);
    queue->head = node;
    if(last_head != NULL) {
        last_head->next = node;
    }
    if(queue->tail == NULL) {
        queue->tail = node;
    }
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->lock);
}

void xqueue_dequeue(xqueue_t *queue, void **output) {
    pthread_mutex_lock(&queue->lock);
    if(queue->tail == NULL) {
        pthread_cond_wait(&queue->cond, &queue->lock);
    }
    xquque_node_t *tail = queue->tail;
    (*output) = tail->data;
    queue->tail = tail->next;
    free(tail);
    pthread_mutex_unlock(&queue->lock);
}

void xqueue_destroy(xqueue_t *queue) {
    pthread_mutex_lock(&queue->lock);
    while (1){
        void *out;
        xqueue_dequeue(queue, &out);
        if(out == NULL) {
            break;
        }
        free(out);
    }
    pthread_mutex_unlock(&queue->lock);
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->cond);
    free(queue);
}