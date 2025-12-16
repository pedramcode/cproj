#ifndef CORE_CONFIG_H
#define CORE_CONFIG_H

typedef struct {
    char host[64];
    int port;
} node_addr_t;

typedef struct {
    int port;
    node_addr_t *seed_nodes;
    int seed_nodes_count;
} config_t;

#endif // CORE_CONFIG_H