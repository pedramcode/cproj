#include "transfer/udp.h"
#include <string.h>
#include <malloc.h>
#include <fcntl.h>

#define WORKERS 16
void* __worker_function();

void udp_server_new(udp_server_t **server, int port, void (*logger) (const char* fmt, ...)){
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

    bind((*server)->fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(logger) logger("UDP Server initialized [::]:%d", port);
    (*server)->logger = logger;

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
            // Do work ...
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
    if(server->logger) server->logger("UDP Server is running");

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

            if(server->logger) {
                server->logger("Enqueued UDP request of %d bytes", n);
            }
        }
    }
}