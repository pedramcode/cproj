#include "helpers/convert.h"

#include <stddef.h>
#include <malloc.h>

void cvt_sha1_to_string(char* hash, char **output) {
    (*output) = malloc(sizeof(char) * 41);
    size_t offset;
    for( offset = 0; offset < 20; offset++) {
        sprintf( ( (*output) + (2*offset)), "%02x", hash[offset]&0xff);
    }
}

size_t cvt_bytes_to_hex(const char *data, size_t len, char **output) {
    const size_t per_byte = 5;
    size_t out_len = len * per_byte;

    *output = malloc(out_len + 1);
    if (!*output)
        return 0;

    char *p = *output;

    for (size_t i = 0; i < len; ++i) {
        unsigned char b = (unsigned char)data[i];
        int written = sprintf(p, "0x%02X", b);
        p += written;

        if (i + 1 < len) {
            *p++ = ' ';
        }
    }

    *p = '\0';
    return (p - *output);
}