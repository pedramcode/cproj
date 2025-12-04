#include "node/request_handler.h"

#include <stdio.h>
#include <malloc.h>
#include <memory.h>

#include "core/global.h"
#include "packet/packet.h"
#include "packet/types.h"

/// @brief receives incomming requests from UDP server and process them
/// @param data incomming UDP data
/// @param data_length incomming UPD data size
/// @param output result of processing packet
/// @return size of output
size_t node_request_handler(char* data, size_t data_length, char **output) {
    packet_t *packet;
    packet_from_bytes(&packet, data, data_length);

    if(packet == NULL){
        packet_t *p = malloc(sizeof(packet_t));
        p->payload = NULL;
        p->payload_len = 0;
        p->req_id = 0;
        memcpy(p->sender_node_hash, global_node->hash, 20);
        p->type = CMD_ERROR;
        p->version = 1;
        size_t s = packet_to_bytes(p, output);
        packet_destroy(p);
        return s;
    }

    char *packet_str;
    packet_to_string(packet, &packet_str);
    printf("PACKET: %s\n", packet_str);
    return 0;
}
