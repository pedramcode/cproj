#include "node/commands.h"

size_t (*COMMAND_DISPATCHER[256]) (packet_t *packet, char **output);

size_t _empty_command(packet_t *packet, char **output) {
    (void)packet;
    packet_t *p;
    packet_new(1, CMD_ERROR, 0, NULL, 0, &p);
    size_t len = packet_to_bytes(p, output);
    packet_destroy(p);
    return len;
}

void load_commands() {
    for(int i = 0 ; i < 256 ; i++) {
        COMMAND_DISPATCHER[i] = _empty_command;
    }
    COMMAND_DISPATCHER[CMD_PING] = command_ping;
}