#include "core/args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers/ini.h"
#include "core/config.h"
#include "core/global.h"

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    config_t* cfg = (config_t*)user;
    cfg->port = 1998;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("SERVER", "PORT")) {
        cfg->port = atoi(value);
    // } else if (MATCH("user", "name")) {
    //     pconfig->name = strdup(value);
    // } else if (MATCH("user", "email")) {
    //     pconfig->email = strdup(value);
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

    ini_parse_file(file, handler, &global_config);

    fclose(file);
}