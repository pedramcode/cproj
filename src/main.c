#include <signal.h>

#include "core/global.h"
#include "core/signal.h"
#include "node/node.h"

int main(){
    // setup signal handers
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);
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
