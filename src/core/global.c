#include "core/global.h"
#include "helpers/default_logger.h"

/// @brief global node instance
node_t *global_node = NULL;

/// @brief node state
node_state global_state = NODEST_RUNNING;

config_t global_config;

lookup_table_t *global_lookup = NULL;

void (*globall_logger)(const char* fmt, ...) = default_logger;