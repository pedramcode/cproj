#ifndef TRANSFER_UDP_H
#define TRANSFER_UDP_H

#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "helpers/xqueue.h"

typedef struct {
    struct sockaddr_in addr;
    char *data;
    int data_length;
} udp_request_t;

typedef struct {
    int port;
    int fd;
    xqueue_t *queue;
    pthread_t *workers;
    size_t (*request_handler) (char* data, size_t data_length, char **output);
} udp_server_t;

void udp_server_new(udp_server_t **server, int port);
void udp_server_destroy(udp_server_t *server);
void udp_server_run(udp_server_t *server);
void udp_server_set_request_handler(udp_server_t *server, size_t (*request_handler) (char* data, size_t data_length, char **output));

#endif // TRANSFER_UDP_H