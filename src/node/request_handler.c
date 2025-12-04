#include "node/request_handler.h"

#include <stdio.h>
#include <malloc.h>
#include <memory.h>

size_t node_request_handler(char* data, size_t data_length, char **output) {
    printf("Received Data: %s\n", data);
    printf("Received Data Length: %ld\n", data_length);
    
    char *message = "Hello From Server\n";
    (*output) = malloc(strlen(message));
    memcpy((*output), message, strlen(message));
    return strlen(message);
}
