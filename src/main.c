#include <stdlib.h>
#include <stdio.h>

#include "transfer/udp.h"
#include "helpers/default_logger.h"
#include "helpers/xqueue.h"

int main(){
    // udp_server_t *server;
    // udp_server_new(&server, 1998, default_logger);
    // udp_server_run(server);
    // udp_server_destroy(server);
    xqueue_t *q;
    xqueue_new(&q);
    int *x = (int*) malloc(sizeof(int));
    (*x) = 10;
    xqueue_enqueue(q, (void*) x);
    int *y = (int*) malloc(sizeof(int));
    (*y) = 20;
    xqueue_enqueue(q, (void*) y);
    int *z = (int*) malloc(sizeof(int));
    (*z) = 30;
    xqueue_enqueue(q, (void*) z);

    for(int i = 0 ; i < 3 ; i++){
        int *output;
        xqueue_dequeue(q, (void**) &output);
        printf("OUTPUT: %d\n", *output);
        free(output);
    }

    xqueue_destroy(q);
    return 0;
}