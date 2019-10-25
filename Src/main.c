#include "main.h"

int main(){
    DeviceInit();
    //wait until usartinit complete
    while(UsartInit());
    char usartInitCompleteString[] = "Usart init complete";
    UsartSendData((uint8_t*)usartInitCompleteString, sizeof(usartInitCompleteString));

    while(1){
        uint8_t temp;
        if(!UsartReadBuffer(&temp)){
            UsartSendData(&temp, 1);
        }
    }
    return 0;
}
