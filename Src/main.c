#include "main.h"

void ProcessBluetoothData(char data);

int main()
{
    SysInit();

    uint8_t data;
    while(1) {
        SetMystdioTarget(USART2);
        if(!MystdinBufferIsEmpty()){
            data = MyGetchar();
            SetMystdioTarget(USART1);
            MyPutchar(data);
        }
        SetMystdioTarget(USART1);
        if(!MystdinBufferIsEmpty()){
            data = MyGetchar();
            SetMystdioTarget(USART2);
            MyPutchar(data);
            ProcessBluetoothData(data);
        }
    }
    return 0;
}

char buffer[6];
int received = 0;

void ProcessBluetoothData(char data)
{
    if(data == '@') {
        received = 1;
    }
    else if(received == 7) {
        if(data == '$') {
            color c;
            HexToColor(buffer, &c);

            // setColor(c);
        }

        received = 0;
    }
    else if(received) {
        buffer[received - 1] = data;
        received++;
    }
}
