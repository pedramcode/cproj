#include "transfer/udp.h"
#include <string.h>
#include <malloc.h>
#include <fcntl.h>

#include "core/global.h"

#define WORKERS 8
void* __worker_function();

void udp_server_new(udp_server_t **server, int port){
    *server = malloc(sizeof(udp_server_t));
    (*server)->port = port;

    struct sockaddr_in servaddr;
    (*server)->fd = socket(AF_INET, SOCK_DGRAM, 0);
    int rcvbuf = 4 * 1024 * 1024;
    setsockopt((*server)->fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));
    fcntl((*server)->fd, F_SETFL, O_NONBLOCK);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    servaddr.sin_family = AF_INET;

    (*server)->request_handler = NULL;

    bind((*server)->fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    globall_logger("UDP Server initialized [::]:%d", port);

    xqueue_new(&(*server)->queue);

    (*server)->workers = malloc(sizeof(pthread_t) * WORKERS);
    for(int i = 0 ; i < WORKERS ; i++) {
        pthread_create(&(*server)->workers[i], NULL, __worker_function, (void*) *server);
    }
}

void* __worker_function(void *server_ptr){
    udp_server_t *server = (udp_server_t*) server_ptr;

    while(1){
        udp_request_t *req = NULL;
        xqueue_dequeue(server->queue, (void**)&req);

        if(req) {
            char *output = NULL;

            if(server->request_handler != NULL){
                size_t len = server->request_handler(req->data, req->data_length, &output);
                sendto(server->fd, output, len, 0, (struct sockaddr*) &req->addr, sizeof(req->addr));
            }
            
            if(output != NULL){
                free(output);
            }
            free(req->data);
            free(req);
        }
    }
    return NULL;
}

void udp_server_destroy(udp_server_t *server){
    xqueue_destroy(server->queue);
    for(int i = 0 ; i < WORKERS ; i++) {
        pthread_cancel(server->workers[i]);
        pthread_join(server->workers[i], NULL);
    }
    free(server->workers);
    free(server);
}

void udp_server_run(udp_server_t *server){
    globall_logger("UDP Server is running");

    char buffer[1024 * 64]; // 64 KB maximum body payload

    while(1) {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int n = recvfrom(server->fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &len);

        if(n > 0) {
            udp_request_t *req = malloc(sizeof(udp_request_t));
            req->addr = addr;
            req->data_length = n;

            req->data = malloc(n);
            memcpy(req->data, buffer, n);

            xqueue_enqueue(server->queue, req);
        }
    }
}

void udp_server_set_request_handler(udp_server_t *server, size_t (*request_handler) (char* data, size_t data_length, char **output)){
    server->request_handler = request_handler;
}