#ifndef CORE_GLOBAL_H
#define CORE_GLOBAL_H

#include "node/node.h"

typedef enum {
    NODEST_RUNNING = 1,
    NODEST_STOPPING,
} node_state;

extern node_t *global_node;
extern node_state global_state;

#endif // CORE_GLOBAL_H