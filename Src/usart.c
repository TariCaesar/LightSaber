#include "usart.h"

typedef struct {
    uint8_t data[USART_RX_BUFFER_SIZE];
    uint32_t head;
    uint32_t tail;
} UsartRxBuffer;

typedef struct {
    uint8_t data[USART_TX_BUFFER_SIZE];
    uint32_t head;
    uint32_t tail;
} UsartTxBuffer;

static UsartRxBuffer usart1RxBuffer;
static UsartTxBuffer usart1TxBuffer;
static UsartRxBuffer usart2RxBuffer;
static UsartTxBuffer usart2TxBuffer;

int32_t UsartInit()
{
    //check the GPIOA clock status
    //if not open, open it
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOA)) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    }

    //GPIO AF configuration
    //USART1 Tx is PA9, USART2 Tx is PA2, configure into af output
    LL_GPIO_InitTypeDef usartGpioInit;
    usartGpioInit.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_9;
    usartGpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
    usartGpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    usartGpioInit.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
    usartGpioInit.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOA, &usartGpioInit);

    //USART1 Rx is PA10, USART2 Rx is PA3, configure into float input
    usartGpioInit.Pin = LL_GPIO_PIN_3 | LL_GPIO_PIN_10;
    usartGpioInit.Mode = LL_GPIO_MODE_INPUT;
    LL_GPIO_Init(GPIOA, &usartGpioInit);

    //Enable usart1 and usart2 clock
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

    LL_USART_DeInit(USART1);
    LL_USART_DeInit(USART2);

    LL_USART_InitTypeDef usartInit;
    usartInit.BaudRate = 115200u;
    usartInit.DataWidth = LL_USART_DATAWIDTH_8B;
    usartInit.StopBits = LL_USART_STOPBITS_1;
    usartInit.Parity = LL_USART_PARITY_NONE;
    usartInit.TransferDirection = LL_USART_DIRECTION_TX_RX;
    usartInit.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    usartInit.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART1, &usartInit);
    LL_USART_Init(USART2, &usartInit);

    //Set usart into asysn mode
    LL_USART_ConfigAsyncMode(USART1);
    LL_USART_ConfigAsyncMode(USART2);

    //Init usart rx buffer
    usart1RxBuffer.head = 0;
    usart1RxBuffer.tail = 0;
    usart2RxBuffer.head = 0;
    usart2RxBuffer.tail = 0;

    //Init usart tx buffer
    usart1TxBuffer.head = 0;
    usart1TxBuffer.tail = 0;
    usart2TxBuffer.head = 0;
    usart2TxBuffer.tail = 0;

    //Init interrupt
    LL_USART_ClearFlag_RXNE(USART1);
    LL_USART_EnableIT_RXNE(USART1);
    LL_USART_ClearFlag_TC(USART1);
    LL_USART_EnableIT_TC(USART1);

    LL_USART_ClearFlag_RXNE(USART2);
    LL_USART_EnableIT_RXNE(USART2);
    LL_USART_ClearFlag_TC(USART2);
    LL_USART_EnableIT_TC(USART2);

    //Enable USART in NVIC
    NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(2, 2, 0));
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(2, 2, 0));
    NVIC_EnableIRQ(USART2_IRQn);

    //Enable Usart
    LL_USART_Enable(USART1);
    LL_USART_Enable(USART2);

    UsartSetMystdioHandler(USART2);
    MyPrintf("USART initialization succeed!\n");
    return 0;
}

static uint32_t UsartReceiveData(uint8_t* addrDst, uint32_t size, USART_TypeDef* usartTarget)
{
    uint32_t i, rxReceiveRemain;
    UsartRxBuffer* rxBuffer;

    if(usartTarget == USART1)
        rxBuffer = &usart1RxBuffer;
    else if(usartTarget == USART2)
        rxBuffer = &usart2RxBuffer;
    else
        return 0;

    rxReceiveRemain =
        (rxBuffer->head < rxBuffer->tail) ? (rxBuffer->head + USART_RX_BUFFER_SIZE - rxBuffer->tail) : (rxBuffer->head - rxBuffer->tail);
    if(rxReceiveRemain < size) size = rxReceiveRemain;
    for(i = 0; i < size; ++i) {
        addrDst[i] = rxBuffer->data[rxBuffer->tail];
        rxBuffer->tail = (rxBuffer->tail + 1) % USART_RX_BUFFER_SIZE;
    }
    return size;
}

static uint32_t UsartTransmitData(uint8_t* addrSrc, uint32_t size, USART_TypeDef* usartTarget)
{
    uint32_t i, txBufferRemain;
    UsartTxBuffer* txBuffer;

    if(usartTarget == USART1)
        txBuffer = &usart1TxBuffer;
    else if(usartTarget == USART2)
        txBuffer = &usart2TxBuffer;
    else
        return 0;

    //check if buffer is large enough
    txBufferRemain =
        (txBuffer->head < txBuffer->tail) ? (txBuffer->tail - txBuffer->head - 1) : (USART_TX_BUFFER_SIZE - txBuffer->head + txBuffer->tail - 1);
    if(txBufferRemain < size) size = txBufferRemain;
    for(i = 0; i < size; ++i) {
        txBuffer->data[txBuffer->head] = addrSrc[i];
        txBuffer->head = (txBuffer->head + 1) % USART_TX_BUFFER_SIZE;
    }

    if(txBuffer->tail != txBuffer->head) {
        LL_USART_TransmitData8(usartTarget, txBuffer->data[txBuffer->tail]);
        txBuffer->tail = (txBuffer->tail + 1) % USART_TX_BUFFER_SIZE;
    }
    return size;
}

static uint32_t Usart1TransmitHandler(uint8_t* addrSrc, uint32_t size){
    return UsartTransmitData(addrSrc, size, USART1);
}

static uint32_t Usart2TransmitHandler(uint8_t* addrSrc, uint32_t size){
    return UsartTransmitData(addrSrc, size, USART2);
}

static uint32_t Usart1ReceiveHandler(uint8_t* addrDst, uint32_t size){
    return UsartReceiveData(addrDst, size, USART1);
}

static uint32_t Usart2ReceiveHandler(uint8_t* addrDst, uint32_t size){
    return UsartReceiveData(addrDst, size, USART2);
}

int32_t UsartSetMystdioHandler(USART_TypeDef* usartTarget){
    if(usartTarget == USART1){
        SetMystdioTransimitHandler(Usart1TransmitHandler);
        SetMystdioReceiveHandler(Usart1ReceiveHandler);
        return 0;
    }
    else if(usartTarget == USART2){
        SetMystdioTransimitHandler(Usart2TransmitHandler);
        SetMystdioReceiveHandler(Usart2ReceiveHandler);
        return 0;
    }
    else return 1;
}

void USART1_IRQHandler()
{
    if(LL_USART_IsActiveFlag_TC(USART1)) {
        LL_USART_ClearFlag_TC(USART1);
        if(usart1TxBuffer.tail != usart1TxBuffer.head) {
            LL_USART_TransmitData8(USART1, usart1TxBuffer.data[usart1TxBuffer.tail]);
            usart1TxBuffer.tail = (usart1TxBuffer.tail + 1) % USART_TX_BUFFER_SIZE;
        }
    }
    else if(LL_USART_IsActiveFlag_RXNE(USART1)) {
        usart1RxBuffer.data[usart1RxBuffer.head] = LL_USART_ReceiveData8(USART1);
        usart1RxBuffer.head = (usart1RxBuffer.head + 1) % USART_RX_BUFFER_SIZE;
        //if rxBuffer is full, throw the oldest data
        if(usart1RxBuffer.head == usart1RxBuffer.tail) {
            usart1RxBuffer.tail = (usart1RxBuffer.tail + 1) % USART_RX_BUFFER_SIZE;
        }
        //clear the interrupt flag
        LL_USART_ClearFlag_RXNE(USART1);
    }
}

void USART2_IRQHandler()
{
    if(LL_USART_IsActiveFlag_TC(USART2)) {
        LL_USART_ClearFlag_TC(USART2);
        if(usart2TxBuffer.tail != usart2TxBuffer.head) {
            LL_USART_TransmitData8(USART2, usart2TxBuffer.data[usart2TxBuffer.tail]);
            usart2TxBuffer.tail = (usart2TxBuffer.tail + 1) % USART_TX_BUFFER_SIZE;
        }
    }
    else if(LL_USART_IsActiveFlag_RXNE(USART2)) {
        usart2RxBuffer.data[usart2RxBuffer.head] = LL_USART_ReceiveData8(USART2);
        usart2RxBuffer.head = (usart2RxBuffer.head + 1) % USART_RX_BUFFER_SIZE;
        //if rxBuffer is full, throw the oldest data
        if(usart2RxBuffer.head == usart2RxBuffer.tail) {
            usart2RxBuffer.tail = (usart2RxBuffer.tail + 1) % USART_RX_BUFFER_SIZE;
        }
        //clear the interrupt flag
        LL_USART_ClearFlag_RXNE(USART2);
    }
}
