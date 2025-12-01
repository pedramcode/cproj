#ifndef TRANSFER_UDP_H
#define TRANSFER_UDP_H

typedef struct {
    int port;
    int fd;
    void (*logger) (const char* fmt, ...);
} udp_server_t;

void udp_server_new(udp_server_t **server, int port, void (*logger) (const char* fmt, ...));
void udp_server_destroy(udp_server_t *server);
void udp_server_run(udp_server_t *server);

#endif // TRANSFER_UDP_H