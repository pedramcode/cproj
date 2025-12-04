#ifndef HELPERS_CONVERT_H
#define HELPERS_CONVERT_H

#include <stddef.h>

void cvt_sha1_to_string(char* hash, char **output);
size_t cvt_bytes_to_hex(const char *data, size_t len, char **output);

#endif // HELPERS_CONVERT_H