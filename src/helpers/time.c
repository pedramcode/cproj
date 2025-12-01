#include "helpers/time.h"
#include <stdio.h>
#include <time.h>

const char* time_now_string(void) {
    static char buf[9];
    time_t now = time(NULL);
    struct tm t;
    
    localtime_r(&now, &t);
    sprintf(buf, "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
    
    return buf;
}