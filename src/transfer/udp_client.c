#include "transfer/udp_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>

#define UDP_CLIENT_BUFFER_SIZE 65536

int udp_client_new(udp_client_t **client, int timeout_ms) {
    if (!client) return -1;

    (*client) = malloc(sizeof(udp_client_t));
    if (!(*client)) return -1;

    (*client)->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if ((*client)->fd < 0) {
        perror("socket");
        free(*client);
        return -1;
    }

    int flags = fcntl((*client)->fd, F_GETFL, 0);
    fcntl((*client)->fd, F_SETFL, flags | O_NONBLOCK);

    (*client)->timeout_ms = timeout_ms;
    return 0;
}

void udp_client_destroy(udp_client_t *client) {
    if (!client) return;
    close(client->fd);
    free(client);
}

int udp_client_send_packet(udp_client_t *client, const char *dest_ip, int dest_port, const packet_t *request_packet, packet_t **response_packet) {
    if (!client || !dest_ip || !request_packet || !response_packet) return -1;

    // Serialize request packet
    char *request_bytes = NULL;
    size_t request_len = packet_to_bytes(request_packet, &request_bytes);
    if (request_len == 0 || !request_bytes) return -1;

    // Build target address
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(dest_port);
    if (inet_pton(AF_INET, dest_ip, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        free(request_bytes);
        return -1;
    }

    // Send packet
    ssize_t sent = sendto(client->fd, request_bytes, request_len, 0, (struct sockaddr*)&addr, sizeof(addr));
    free(request_bytes);
    if (sent < 0) {
        perror("sendto");
        return -1;
    }

    // Wait for response
    fd_set read_fds;
    struct timeval tv;
    tv.tv_sec = client->timeout_ms / 1000;
    tv.tv_usec = (client->timeout_ms % 1000) * 1000;

    FD_ZERO(&read_fds);
    FD_SET(client->fd, &read_fds);

    int ret = select(client->fd + 1, &read_fds, NULL, NULL, &tv);
    if (ret < 0) {
        perror("select");
        return -1;
    } else if (ret == 0) {
        // Timeout
        return -1;
    }

    if (FD_ISSET(client->fd, &read_fds)) {
        char buffer[UDP_CLIENT_BUFFER_SIZE];
        socklen_t addr_len = sizeof(addr);
        ssize_t n = recvfrom(client->fd, buffer, UDP_CLIENT_BUFFER_SIZE, 0, (struct sockaddr*)&addr, &addr_len);
        if (n < 0) {
            perror("recvfrom");
            return -1;
        }

        packet_from_bytes(response_packet, buffer, n);
        if (*response_packet == NULL) {
            fprintf(stderr, "Failed to deserialize response packet\n");
            return -1;
        }
        return 0;
    }

    return -1;
}
