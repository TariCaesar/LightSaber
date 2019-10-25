#include "main.h"

int main(){
    DeviceInit();
    //wait until usartinit complete
    while(UsartInit());
    char usartInitCompleteString[] = "Usart init complete\n";
    UsartSendData((uint8_t*)usartInitCompleteString, sizeof(usartInitCompleteString));

    while(1){
    }
    return 0;
}
