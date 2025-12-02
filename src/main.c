#include "transfer/udp.h"
#include "helpers/default_logger.h"

int main(){
    udp_server_t *server;
    udp_server_new(&server, 1998, default_logger);
    udp_server_run(server);
    udp_server_destroy(server);
    return 0;
}