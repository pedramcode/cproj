#include <stdio.h>
#include "core/global.h"
#include "node/node.h"

int main(){
    // setup node
    node_t *node;
    node_config_t cfg;
    cfg.port = 1998;
    node_new(&node, cfg);
    global_node = node;
    node_start(node);
    node_destroy(node);
    return 0;
}
