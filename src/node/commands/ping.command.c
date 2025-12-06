#include "commands.h"

#include <malloc.h>
#include <string.h>

void command_ping(packet_t *packet, char **output) {
    packet_t *p = malloc(sizeof(packet_t));
    p->req_id = packet->req_id;
    p->type = packet->type;
    p->version = 1;
    char *pong = "PONG";
    p->payload = malloc(strlen(pong) - 1);
    memcpy(p->payload, pong, strlen(pong) - 1);
    p->payload_len = strlen(pong) - 1;
    return packet_to_bytes(p, output);
}
