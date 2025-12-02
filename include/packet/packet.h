#ifndef PACKET_PACKET_H
#define PACKET_PACKET_H

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint8_t version;
    uint8_t type;
    uint16_t req_id;
    char sender_node_hash[20];
    char receiver_node_hash[20];
    uint16_t payload_len;
    char *payload;
} packet_t;

void packet_from_bytes(packet_t **packet, char *bytes, size_t len);
size_t packet_to_bytes(packet_t *packet, char **output);

#endif // PACKET_PACKET_H
