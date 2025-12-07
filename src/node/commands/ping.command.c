#include "node/commands.h"

#include <malloc.h>
#include <string.h>

size_t command_ping(packet_t *packet, char **output) {
    packet_t *p;
    char *pong = "PONG";
    packet_new(1, CMD_PING, packet->req_id, pong, strlen(pong), &p);
    size_t len = packet_to_bytes(p, output);
    packet_destroy(p);
    return len;
}
