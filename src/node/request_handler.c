#include "node/request_handler.h"

#include <stdio.h>
#include <malloc.h>
#include <memory.h>

#include "core/global.h"
#include "packet/packet.h"
#include "packet/types.h"
#include "node/commands.h"

/// @brief receives incomming requests from UDP server and process them
/// @param data incomming UDP data
/// @param data_length incomming UPD data size
/// @param output result of processing packet
/// @return size of output
size_t node_request_handler(char* data, size_t data_length, char **output) {
    packet_t *packet;
    packet_from_bytes(&packet, data, data_length);

    size_t len = 0;
    if(packet == NULL){
        packet_t *p;
        packet_new(1, CMD_ERROR, 0, NULL, 0, &p);
        packet_to_bytes(p, output);
        packet_destroy(p);
        goto end;
    }

    switch(packet->type) {
        case CMD_PING: { len = command_ping(packet, output); break; }
        default: {
            packet_t *p;
            packet_new(1, CMD_ERROR, 0, NULL, 0, &p);
            packet_to_bytes(p, output);
            packet_destroy(p);
            goto end;
        }
    }

    end:
    packet_destroy(packet);
    return len;
}
