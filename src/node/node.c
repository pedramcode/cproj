#include "node/node.h"

#include <malloc.h>
#include <string.h>
#include "helpers/default_logger.h"
#include "helpers/network.h"
#include "helpers/sha1.h"

void node_new(node_t **node, node_config_t config){
    (*node) = malloc(sizeof(node_t));
    udp_server_new(&(*node)->server, config.port, default_logger);

    char mac[18];
    get_mac_address_sysfs("eth0", mac);
    char mac_port[64];
    sprintf(mac_port, "%s:%d", mac, config.port);

    char *result = malloc(sizeof(char) * 20);
    SHA1(result, mac_port, strlen(mac_port));
    (*node)->hash = result;

    (*node)->hash_hex_string = malloc(sizeof(char) * 41);
    size_t offset;
    for( offset = 0; offset < 20; offset++) {
        sprintf( ( (*node)->hash_hex_string + (2*offset)), "%02x", result[offset]&0xff);
    }
    if ((*node)->server->logger) (*node)->server->logger("Node is ready [%s]", (*node)->hash_hex_string);
}

void node_start(node_t *node){
    udp_server_run(node->server);
}

void node_destroy(node_t *node){
    udp_server_destroy(node->server);
    free(node->hash_hex_string);
    free(node->hash);
    free(node);
}
