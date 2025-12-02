#include <stdio.h>
#include <string.h>

int get_mac_address_sysfs(const char *iface, char mac[18]) {
    char path[128];
    FILE *fp;

    snprintf(path, sizeof(path), "/sys/class/net/%s/address", iface);

    fp = fopen(path, "r");
    if (!fp)
        return -1;

    if (!fgets(mac, 18, fp)) {
        fclose(fp);
        return -1;
    }

    fclose(fp);

    size_t len = strlen(mac);
    if (len > 0 && mac[len - 1] == '\n')
        mac[len - 1] = '\0';

    return 0;
}
