#ifndef CORE_GLOBAL_H
#define CORE_GLOBAL_H

#include "node/node.h"
#include "node/lookup_table.h"
#include "core/config.h"

typedef enum {
    NODEST_RUNNING = 1,
    NODEST_STOPPING,
} node_state;

extern node_t *global_node;
extern node_state global_state;
extern config_t global_config;
extern lookup_table_t *global_lookup;
extern void (*globall_logger)(const char* fmt, ...);

#endif // CORE_GLOBAL_H