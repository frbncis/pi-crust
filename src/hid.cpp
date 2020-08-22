#include "hid.h"

void send_key(FILE *hid_dev, unsigned short key, unsigned short mod)
{
    if (hid_dev != NULL)
    {
        fprintf(hid_dev, "%c%c%c%c%c%c%c%c", mod, '\0', key, '\0', '\0', '\0', '\0', '\0');
    }
    else
    {
        fprintf(stderr, "Cannot write to device\n");
    }
}