#include <stdio.h>
#include <string.h>
#include <openssl/rand.h>

int load_or_generate_node_id(uint8_t id[20], const char* filename) {
    FILE *fp = fopen(filename, "rb");

    if (fp != NULL) {
        size_t r = fread(id, 1, 20, fp);
        fclose(fp);
        if (r == 20) return 0;
    }

    // Generate a secure 160-bit random ID
    if (RAND_bytes(id, 20) != 1) {
        return -1; // crypto error
    }

    // Persist it
    fp = fopen(filename, "wb");
    if (!fp) return -1;
    fwrite(id, 1, 20, fp);
    fclose(fp);

    return 0;
}