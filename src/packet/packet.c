#include "packet/packet.h"

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "helpers/convert.h"

void packet_from_bytes(packet_t **packet, const char *bytes, size_t len) {
    /* Minimum valid size must include the fixed header */
    const size_t HEADER_SIZE = 26;

    if (len < HEADER_SIZE) {
        *packet = NULL;
        return;
    }

    /* Allocate packet structure */
    *packet = malloc(sizeof(packet_t));
    packet_t *pack = *packet;

    /* Extract basic fields */
    pack->version = (uint8_t)bytes[0];
    pack->type    = (uint8_t)bytes[1];

    /* req_id is stored as network-byte-order 16-bit */
    uint16_t req_raw;
    memcpy(&req_raw, bytes + 2, sizeof(uint16_t));
    pack->req_id = ntohs(req_raw);

    /* Copy sender hash (20 bytes, raw binary, not null-terminated) */
    memcpy(pack->sender_node_hash, bytes + 4, 20);

    /* Extract payload size */
    uint16_t payload_raw;
    memcpy(&payload_raw, bytes + 24, sizeof(uint16_t));
    pack->payload_len = ntohs(payload_raw);

    /* Check that buffer contains the full payload */
    if (len < HEADER_SIZE + pack->payload_len) {
        free(pack);
        *packet = NULL;
        return;
    }

    /* Copy payload */
    if (pack->payload_len > 0) {
        pack->payload = malloc(pack->payload_len);
        memcpy(pack->payload, bytes + HEADER_SIZE, pack->payload_len);
    } else {
        pack->payload = NULL;
    }
}

size_t packet_to_bytes(const packet_t *packet, char **output) {
    const size_t HEADER_SIZE = 26;
    size_t total_len = HEADER_SIZE + packet->payload_len;

    /* Allocate output buffer */
    *output = malloc(total_len);
    char *buf = *output;

    /* Serialize version & type */
    buf[0] = packet->version;
    buf[1] = packet->type;

    /* req_id (network order) */
    uint16_t req_raw = htons(packet->req_id);
    memcpy(buf + 2, &req_raw, sizeof(uint16_t));

    /* sender hash (20 bytes) */
    memcpy(buf + 4, packet->sender_node_hash, 20);

    /* payload length (network order) */
    uint16_t payload_raw = htons(packet->payload_len);
    memcpy(buf + 24, &payload_raw, sizeof(uint16_t));

    /* payload */
    if (packet->payload_len > 0) {
        memcpy(buf + HEADER_SIZE, packet->payload, packet->payload_len);
    }

    return total_len;
}

void packet_destroy(packet_t *packet) {
    if (!packet) return;
    if (packet->payload) {
        free(packet->payload);
    }
    free(packet);
}

void packet_to_string(const packet_t *packet, char **output) {
    /* Allocate debug string (static size for simplicity) */
    *output = malloc(512);

    char *sender_hash_output;
    cvt_sha1_to_string((char*)packet->sender_node_hash, &sender_hash_output);

    char *payload_output;
    cvt_bytes_to_hex(packet->payload, packet->payload_len, &payload_output);

    sprintf(
        *output,
        "V=[%u] T=[%u] REQ=[%u] SENDER=[%s] PAYLEN=[%u] PAYLOAD=[%s]",
        packet->version,
        packet->type,
        packet->req_id,
        sender_hash_output,
        packet->payload_len,
        payload_output
    );

    free(sender_hash_output);
    free(payload_output);
}
