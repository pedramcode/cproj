#ifndef TRANSFER_UDP_CLIENT_H
#define TRANSFER_UDP_CLIENT_H

#include <stdint.h>
#include <stddef.h>
#include "packet/packet.h"

typedef struct {
    int fd;                 // UDP socket
    int timeout_ms;          // Timeout for response in milliseconds
} udp_client_t;

/// @brief Initialize a UDP client (generic, can send to any IP:port)
/// @param client Pointer to udp_client_t pointer
/// @param timeout_ms Response timeout in milliseconds
/// @return 0 on success, -1 on failure
int udp_client_new(udp_client_t **client, int timeout_ms);

/// @brief Destroy UDP client and release resources
/// @param client Pointer to udp_client_t
void udp_client_destroy(udp_client_t *client);

/// @brief Send a packet to a remote node and wait for its response
/// @param client Pointer to udp_client_t
/// @param dest_ip Destination IPv4 string
/// @param dest_port Destination port
/// @param request_packet Input packet to send
/// @param response_packet Output packet received (malloced, must be destroyed)
/// @return 0 on success, -1 on timeout or error
int udp_client_send_packet(udp_client_t *client, const char *dest_ip, int dest_port, const packet_t *request_packet, packet_t **response_packet);

#endif // TRANSFER_UDP_CLIENT_H
