#include "stm32f103xb.h"

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color;

void HexToColor(const char *hexString, color *result);
