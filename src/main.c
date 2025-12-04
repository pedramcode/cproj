#include <stdio.h>
#include "node/node.h"

int main(int argc, char **argv){
    // setup node
    node_t *node;
    node_config_t cfg;
    cfg.port = 1998;
    node_new(&node, cfg);
    node_start(node);
    node_destroy(node);
    return 0;
}
