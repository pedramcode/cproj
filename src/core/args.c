#include "core/args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "helpers/ini.h"
#include "core/config.h"
#include "core/global.h"

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    config_t* cfg = (config_t*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    
    if (MATCH("SERVER", "PORT")) {
        cfg->port = atoi(value);
    } else if (MATCH("SERVER", "SEEDS")) {
        // First, count how many seeds we have by counting spaces
        char *value_copy = strdup(value);
        char *ptr = value_copy;
        int count = 1;
        
        while (*ptr) {
            if (isspace((unsigned char)*ptr)) {
                count++;
            }
            ptr++;
        }
        
        // Allocate memory for seed nodes
        cfg->seed_nodes = malloc(count * sizeof(node_addr_t));
        cfg->seed_nodes_count = 0;
        
        // Reset pointer and parse each seed
        ptr = value_copy;
        char *token = strtok(ptr, " \t\n\r");
        
        while (token != NULL && cfg->seed_nodes_count < count) {
            // Parse host:port format
            char *colon = strchr(token, ':');
            if (colon != NULL) {
                *colon = '\0'; // Split at colon
                char *host = token;
                char *port_str = colon + 1;
                
                // Copy host (ensure it fits in the buffer)
                strncpy(cfg->seed_nodes[cfg->seed_nodes_count].host, host, 
                       sizeof(cfg->seed_nodes[cfg->seed_nodes_count].host) - 1);
                cfg->seed_nodes[cfg->seed_nodes_count].host[
                    sizeof(cfg->seed_nodes[cfg->seed_nodes_count].host) - 1] = '\0';
                
                // Parse port
                cfg->seed_nodes[cfg->seed_nodes_count].port = atoi(port_str);
                cfg->seed_nodes_count++;
            }
            token = strtok(NULL, " \t\n\r");
        }
        
        free(value_copy);
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

void process_args(int argc, char **argv) {
    (void) argc;
    (void) argv;

    if(argc == 1){
        fprintf(stderr, "*** provide config file path ***\n");
        exit(1);
    }

    FILE *file;
    file = fopen(argv[1], "r");

    if(file == NULL) {
        fprintf(stderr, "*** cannot open config file at \"%s\" ***\n", argv[1]);
        exit(1);
    }

    // Initialize config with default values
    global_config.port = 1998;
    global_config.seed_nodes = NULL;
    global_config.seed_nodes_count = 0;

    ini_parse_file(file, handler, &global_config);

    fclose(file);
}
