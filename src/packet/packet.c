#include "packet/packet.h"

#include <malloc.h>
#include <string.h>
#include <arpa/inet.h>

void packet_from_bytes(packet_t **packet, char *bytes, size_t len) {
    /* Ensure minimal size for header */
    if (len < 46) {
        *packet = NULL;
        return;
    }

    *packet = malloc(sizeof(packet_t));
    packet_t *pack = *packet;

    /* Extract fixed header fields */
    pack->version = (uint8_t)bytes[0];
    pack->type    = (uint8_t)bytes[1];

    /* req_id is 16-bit, network byte order */
    uint16_t req_raw;
    memcpy(&req_raw, bytes + 2, sizeof(uint16_t));
    pack->req_id = ntohs(req_raw);

    /* Copy sender hash (20 bytes, no NULL termination) */
    memcpy(pack->sender_node_hash, bytes + 4, 20);

    /* Copy receiver hash (20 bytes) */
    memcpy(pack->receiver_node_hash, bytes + 24, 20);

    /* Extract payload length */
    uint16_t payload_raw;
    memcpy(&payload_raw, bytes + 44, sizeof(uint16_t));
    pack->payload_len = ntohs(payload_raw);

    /* Validate packet length */
    if (len < 46 + (size_t) pack->payload_len) {
        /* Invalid packet: not enough bytes for payload */
        free(pack);
        *packet = NULL;
        return;
    }

    /* Allocate and copy payload */
    if (pack->payload_len > 0) {
        pack->payload = malloc(pack->payload_len);
        memcpy(pack->payload, bytes + 46, pack->payload_len);
    } else {
        pack->payload = NULL;
    }
}

size_t packet_to_bytes(packet_t *packet, char **output) {
    /* Total length = fixed header (46) + payload */
    size_t total_len = 46 + packet->payload_len;

    /* Allocate output buffer */
    *output = malloc(total_len);
    char *buf = *output;

    /* Serialize fields */
    buf[0] = packet->version;
    buf[1] = packet->type;

    /* req_id is 16-bit in network order */
    uint16_t req_raw = htons(packet->req_id);
    memcpy(buf + 2, &req_raw, sizeof(uint16_t));

    /* Sender/receiver hashes (20 bytes each) */
    memcpy(buf + 4,  packet->sender_node_hash, 20);
    memcpy(buf + 24, packet->receiver_node_hash, 20);

    /* Payload length (network byte order) */
    uint16_t payload_raw = htons(packet->payload_len);
    memcpy(buf + 44, &payload_raw, sizeof(uint16_t));

    /* Payload (may be zero-length) */
    if (packet->payload_len > 0) {
        memcpy(buf + 46, packet->payload, packet->payload_len);
    }

    return 46 + packet->payload_len;
}