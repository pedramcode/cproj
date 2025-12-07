#ifndef PACKET_PACKET_H
#define PACKET_PACKET_H

#include <stdint.h>
#include <stdlib.h>
#include "packet/types.h"

typedef struct {
    uint8_t  version;
    uint8_t  type;
    uint16_t req_id;
    char     sender_node_hash[20];
    uint16_t payload_len;
    char    *payload;
} packet_t;

/* Deserialize bytes into a packet_t. On failure, *packet becomes NULL. */
void packet_from_bytes(packet_t **packet, const char *bytes, size_t len);

/* Serialize packet_t into bytes. Returns total size written. */
size_t packet_to_bytes(const packet_t *packet, char **output);

/* Free internal memory of packet */
void packet_destroy(packet_t *packet);

/* Produce a printable string representation (debugging use). */
void packet_to_string(const packet_t *packet, char **output);

void packet_new(uint8_t version, packet_type_t type, uint16_t req_id, char *payload, uint16_t payload_length, packet_t **output_packet);

#endif // PACKET_PACKET_H
