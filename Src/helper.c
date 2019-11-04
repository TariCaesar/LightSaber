#include <string.h>
#include <stdlib.h>
#include "helper.h"

void HexToColor(const char *hexString, color *result)
{
    if (strlen(hexString) != 6)
    {
        return;
    }

    int dec = (int)strtol(hexString, NULL, 16);
    result->r = (dec >> 16) & 0xFF;
    result->g = (dec >> 8) & 0xFF;
    result->b = dec & 0xFF;
}
