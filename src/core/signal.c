#include "core/signal.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/global.h"

void signal_handler(int sig) {
    if(global_state == NODEST_STOPPING) return;
    global_state = NODEST_STOPPING;
    char *m1 = "\n";
    write(STDOUT_FILENO, m1, strlen(m1));
    char *m2 = "Gracefully shutting down node...\n";
    write(STDOUT_FILENO, m2, strlen(m2));
    sleep(3);
    exit(sig);
}