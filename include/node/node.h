#ifndef NODE_NODE_H
#define NODE_NODE_H

#include "database/store.h"
#include "transfer/udp.h"

typedef struct {
    udp_server_t *server;
    char hash[20];
    char hash_hex_string[41];
    kv_store_t *storage;
} node_t;

typedef struct {
    int port;
} node_config_t;

void node_new(node_t **node, node_config_t config);
void node_start(node_t *node);
void node_destroy(node_t *node);

#endif // NODE_NODE_H
