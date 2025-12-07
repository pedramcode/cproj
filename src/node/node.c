#include "node/node.h"

#include <malloc.h>
#include <string.h>
#include "helpers/default_logger.h"
#include "helpers/network.h"
#include "helpers/sha1.h"
#include "node/request_handler.h"
#include "helpers/convert.h"

#define BUCKETS 1024

void node_new(node_t **node, node_config_t config){
    (*node) = malloc(sizeof(node_t));
    udp_server_new(&(*node)->server, config.port, default_logger);
    udp_server_set_request_handler((*node)->server, node_request_handler);

    uint8_t node_id[20];
    load_or_generate_node_id(node_id, "node_id.bin");

    char *result = malloc(sizeof(char) * 20);
    SHA1(result, (const char*) node_id, 20);
    memcpy((*node)->hash, result, 20);

    char *string_hash;
    cvt_sha1_to_string(result, &string_hash);
    sprintf((*node)->hash_hex_string, "%s", string_hash);
    free(string_hash);

    if ((*node)->server->logger) (*node)->server->logger("Node is ready [%s]", (*node)->hash_hex_string);
    kv_store_new(&(*node)->storage, BUCKETS);

    free(result);
}

void node_start(node_t *node){
    udp_server_run(node->server);
}

void node_destroy(node_t *node){
    udp_server_destroy(node->server);
    kv_store_destroy(node->storage);
    free(node);
}
