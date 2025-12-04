#include "helpers/xqueue.h"

#include <malloc.h>

void xqueue_new(xqueue_t **queue) {
    *queue = malloc(sizeof(xqueue_t));
    xqueue_node_t *dummy = malloc(sizeof(xqueue_node_t));
    dummy->data = NULL;
    dummy->next = NULL;
    (*queue)->head = dummy;
    (*queue)->tail = dummy;
    pthread_mutex_init(&(*queue)->lock, NULL);
    pthread_cond_init(&(*queue)->cond, NULL);
}

void xqueue_enqueue(xqueue_t *queue, void *data) {
    xqueue_node_t *node = malloc(sizeof(xqueue_node_t));
    node->data = data;
    node->next = NULL;

    pthread_mutex_lock(&queue->lock);
    queue->tail->next = node;
    queue->tail = node;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->lock);
}

void xqueue_dequeue(xqueue_t *queue, void **output) {
    pthread_mutex_lock(&queue->lock);
    while(queue->head->next == NULL) {
        pthread_cond_wait(&queue->cond, &queue->lock);
    }
    xqueue_node_t *node = queue->head->next;
    *output = node->data;
    queue->head->next = node->next;
    if(queue->tail == node) {
        queue->tail = queue->head; // reset tail to dummy if last node removed
    }
    free(node);
    pthread_mutex_unlock(&queue->lock);
}

void xqueue_destroy(xqueue_t *queue) {
    pthread_mutex_lock(&queue->lock);
    xqueue_node_t *node = queue->head;
    while(node) {
        xqueue_node_t *next = node->next;
        free(node);
        node = next;
    }
    pthread_mutex_unlock(&queue->lock);
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->cond);
    free(queue);
}
