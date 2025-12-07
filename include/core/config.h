#ifndef CORE_CONFIG_H
#define CORE_CONFIG_H

typedef struct {
    int port;
    char **seeds;
    unsigned seeds_count;
} config_t;

#endif // CORE_CONFIG_H