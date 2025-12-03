// #include <stdio.h>
// #include "node/node.h"

#include <string.h>
#include <stdio.h>
#include "database/store.h"

int main(){
    // node_t *node;
    // node_config_t cfg;
    // cfg.port = 1998;
    // node_new(&node, cfg);
    // node_start(node);
    // node_destroy(node);

    kv_store_t *store;
    kv_store_new(&store, 8);
    {
        char *value = "Hello World!";
        char *key = "hello";
        kv_store_put(store, key, strlen(key), value, strlen(value));
    }
    {
        char *value = "This is a test";
        char *key = "Pedram made this";
        kv_store_put(store, key, strlen(key), value, strlen(value));
    }
    {
        char *value = "Name";
        char *key = "Pedram";
        kv_store_put(store, key, strlen(key), value, strlen(value));
    }

    char *res;
    char *key = "hello";
    kv_store_get(store, key, strlen(key), &res);
    if(res != NULL){
        printf("Value: %s\n", res);
    } else {
        printf("No value found\n");
    }

    return 0;
}
