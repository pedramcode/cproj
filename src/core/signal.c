#include "core/signal.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void signal_handler(int sig) {
    char *m1 = "\n";
    write(STDOUT_FILENO, m1, strlen(m1));
    sleep(1);
    char *m2 = "Gracefuly shutting down node...\n";
    write(STDOUT_FILENO, m2, strlen(m2));
    sleep(3);
    exit(sig);
}