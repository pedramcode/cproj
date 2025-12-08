#ifndef NODE_COMMANDS_PING_H
#define NODE_COMMANDS_PING_H

#include "packet/packet.h"

size_t command_ping(packet_t *packet, char **output);

extern size_t (*COMMAND_DISPATCHER[256]) (packet_t *packet, char **output);

void load_commands();

#endif // NODE_COMMANDS_PING_H