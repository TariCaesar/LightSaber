#include "bluetooth.h"
#include <stdlib.h>
#include <string.h>

static uint8_t buffer[6];
static int32_t received = 0;

void HexToColor(const uint8_t* hexString, LED_COLOR* result)
{
    if(strlen(hexString) != 6) {
        return;
    }

    int dec = (int)strtol(hexString, NULL, 16);
    result->R = (dec >> 16) & 0xFF;
    result->G = (dec >> 8) & 0xFF;
    result->B = dec & 0xFF;
}


void ProcessBluetoothData(uint8_t data)
{
    if(data == '@') {
        received = 1;
    }
    else if(received == 7) {
        if(data == '$') {
            LED_COLOR c;
            HexToColor(buffer, &c);

            for(int32_t i = 0; i < LED_NUM; ++i){
                ledColorData[i] = c;
            }
        }
        received = 0;
    }
    else if(received) {
        buffer[received - 1] = data;
        received++;
    }
}
