#include <signal.h>
#include <stdio.h>
#include <string.h>

#include "core/global.h"
#include "core/args.h"
#include "core/signal.h"
#include "node/node.h"
#include "node/lookup_table.h"

int main(int argc, char **argv){
    // config ====================
    process_args(argc, argv);
    
    // setup signal handers ====================
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);

    // initialize ====================
    global_lookup = lookup_table_create();

    // setup node ====================
    node_t *node;
    node_config_t cfg;
    cfg.port = global_config.port;
    node_new(&node, cfg);
    global_node = node;

    // handshaking ====================
    for(int i = 0 ; i < global_config.seed_nodes_count ; i++) {
        char *host = global_config.seed_nodes[i].host;
        int port = global_config.seed_nodes[i].port;

        // communicate with first node respondes to "PING"
        packet_t *req = NULL;
        packet_t *res = NULL;

        packet_new(1, CMD_PING, 1, NULL, 0, &req);
        
        udp_client_t *client;
        udp_client_new(&client, 1000);

        udp_client_send_packet(client, host, port, req, &res);

        if(res != NULL && memcmp("PONG", res->payload, 4) == 0) {
            globall_logger("handshaking with [%s:%d]", host, port);
            goto label_exit;
        }
        globall_logger("seed node %s:%d is offline", host, port);

        udp_client_destroy(client);
        packet_destroy(req);
        packet_destroy(res);
        continue;

        label_exit:
        udp_client_destroy(client);
        packet_destroy(req);
        packet_destroy(res);
        break;
    }

    // start server ====================
    node_start(node);

    // destroy ====================
    node_destroy(node);
    lookup_table_destroy(global_lookup);

    return 0;
}
