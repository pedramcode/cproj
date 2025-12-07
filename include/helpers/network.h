#ifndef HELPERS_NETWORK_H
#define HELPERS_NETWORK_H

#include <stdlib.h>
#include <stdint.h>

int load_or_generate_node_id(uint8_t id[20], const char* filename);

#endif // HELPERS_NETWORK_H