#include "transfer/udp.h"
#include <sys/socket.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <fcntl.h>

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
}

void udp_server_destroy(udp_server_t *server){
    free(server);
}

void udp_server_run(udp_server_t *server){
    if(server->logger) server->logger("UDP Server is running");
    struct sockaddr_in addr;
    int len = sizeof(addr);
    char buffer[256];
    while (1) {
        int n = recvfrom(server->fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, (socklen_t*)&len);
        if(n > 0){
            buffer[n] = '\0';
            if(server->logger) server->logger("RECV: %s", buffer);
        }
    }
}
