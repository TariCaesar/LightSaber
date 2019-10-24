#include "usart.h"

static UsartSendRequest usart1SendRequest;
static RxBuffer rxBuffer;

int UsartInit(){
    //check the GPIOA clock status
    //if not open, open it
    if(LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOA)){
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    }
    //GPIO AF configuration
    //USART1 Tx is PA9, Rx is PA10
    LL_GPIO_InitTypeDef usart1GpioInit;
    usart1GpioInit.Pin = LL_GPIO_PIN_9 | LL_GPIO_PIN_10;
    usart1GpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
    usart1GpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    //Cause we only use 8MHz clok, set gpio to low speed
    usart1GpioInit.Speed = LL_GPIO_SPEED_FREQ_LOW;
    usart1GpioInit.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOA, &usart1GpioInit);
    //Enable usart1 clock
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

    //init usart
    LL_USART_InitTypeDef usartInit;
    //Set usart1 baudrate to 9600
    usartInit.BaudRate = 9600U;
    //Set usart1 frame format 8b data, 1b stop, no parity
    usartInit.DataWidth = LL_USART_DATAWIDTH_8B;
    usartInit.StopBits = LL_USART_STOPBITS_1;
    usartInit.Parity = LL_USART_PARITY_NONE;
    //Set usart into full-duplex
    usartInit.TransferDirection = LL_USART_DIRECTION_TX_RX;
    //Disable hardware flow control
    usartInit.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    //Set oversampling to 16
    usartInit.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART1, &usartInit);

    //init usart clock
    LL_USART_ClockInitTypeDef usartClockInit;
    usartClockInit.ClockOutput = LL_USART_CLOCK_ENABLE;
    usartClockInit.ClockPhase = LL_USART_PHASE_1EDGE;
    usartClockInit.ClockPolarity = LL_USART_POLARITY_HIGH;
    usartClockInit.LastBitClockPulse = LL_USART_LASTCLKPULSE_NO_OUTPUT;
    LL_USART_ClockInit(USART1, &usartClockInit);

    //Init usart send data struct
    usart1SendRequest.addr = 0;
    usart1SendRequest.cnt = 0;
    usart1SendRequest.busy = 0;

    //Init usart cycle buffer
    rxBuffer.head = 0;
    rxBuffer.tail = 0;

    //Init rx interrupt
    LL_USART_ClearFlag_RXNE(USART1);
    LL_USART_EnableIT_RXNE(USART1);
    LL_USART_ClearFlag_TC(USART1);
    LL_USART_EnableIT_TC(USART1);
    //Enable USART1 in NVIC
    NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(2, 2, 0));
    NVIC_EnableIRQ(USART1_IRQn);

    //Enable rx but disable tx
    LL_USART_EnableDirectionRx(USART1);
    LL_USART_DisableDirectionTx(USART1);

    //Enable Usart1 
    LL_USART_Enable(USART1);
    return 0;
}

int UsartSendData(uint8_t *addr_src, uint32_t cnt){
    if(usart1SendRequest.busy)return 1;
    usart1SendRequest.addr = addr_src;
    usart1SendRequest.cnt = cnt;
    usart1SendRequest.busy = 1;
    
    //Put data in TX Reg
    LL_USART_TransmitData8(USART1, *(usart1SendRequest.addr++));

    //Enable USART1 TX
    LL_USART_EnableDirectionTx(USART1);
    return 0;
}

int UsartReadBuffer(uint8_t *addr_dst){
    //judge if the cycle buffer is empty
    if(rxBuffer.head == rxBuffer.tail)return 1;
    *addr_dst = rxBuffer.addr[rxBuffer.tail];
    rxBuffer.tail = (rxBuffer.tail + 1) % RX_BUFFER_SIZE;
    return 0;
}

void USART1_IRQHandler(){
    if(LL_USART_IsActiveFlag_TC(USART1)){
        //clear the interrupt flag
        LL_USART_ClearFlag_TC(USART1);
        usart1SendRequest.cnt--;
        if(usart1SendRequest.cnt){
            LL_USART_TransmitData8(USART1, *(usart1SendRequest.addr++));
        }
        else{
            //Send Complete, Disable Tx and free the bus
            LL_USART_DisableDirectionTx(USART1);
            usart1SendRequest.busy = 0;
        }
    }
    else if(LL_USART_IsActiveFlag_RXNE(USART1)){
        rxBuffer.addr[rxBuffer.head] = LL_USART_ReceiveData8(USART1);
        rxBuffer.head = (rxBuffer.head + 1) % RX_BUFFER_SIZE;
        //if rxBuffer is full, throw the oldest data
        if(rxBuffer.head == rxBuffer.tail){
            rxBuffer.tail = (rxBuffer.tail + 1) % RX_BUFFER_SIZE;
        }
        //clear the interrupt flag
        LL_USART_ClearFlag_RXNE(USART1);
    }
}