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
    uint32_t tailNext;
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
    //init usart
    LL_USART_InitTypeDef usartInit;
    //Set usart1 baudrate to 115200
    usartInit.BaudRate = 115200u;
    //Set usart1 frame format 8b data, 1b stop, no parity
    usartInit.DataWidth = LL_USART_DATAWIDTH_8B;
    usartInit.StopBits = LL_USART_STOPBITS_1;
    usartInit.Parity = LL_USART_PARITY_NONE;
    //only enable RX as we'll enable TX later
    usartInit.TransferDirection = LL_USART_DIRECTION_RX;
    //Disable hardware flow control
    usartInit.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    //Set oversampling to 16
    usartInit.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART1, &usartInit);
    LL_USART_Init(USART2, &usartInit);

    //Set usart into asysn mode
    LL_USART_ConfigAsyncMode(USART1);
    LL_USART_ConfigAsyncMode(USART2);

    //Enable dma clock
    if(!LL_AHB1_GRP1_IsEnabledClock(LL_AHB1_GRP1_PERIPH_DMA1)) {
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    }

    //Init usart rx buffer
    usart1RxBuffer.head = 0;
    usart2RxBuffer.head = 0;

    //Init usart tx buffer
    usart1TxBuffer.head = 0;
    usart1TxBuffer.tail = 0;
    usart1TxBuffer.tailNext = 0;
    usart2TxBuffer.head = 0;
    usart2TxBuffer.tail = 0;
    usart2TxBuffer.tailNext = 0;

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
    NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(2, 2, 1));
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(2, 2, 1));
    NVIC_EnableIRQ(USART2_IRQn);

    //Enable Usart
    LL_USART_Enable(USART1);
    LL_USART_Enable(USART2);

    UsartSetMystdioHandler(USART2);
    MyPrintf("USART initialization succeed!\n");
    return 0;
}

static int32_t UsartTransmitTrigger(USART_TypeDef* usartTarget)
{
    //if buffer remain less than USART_TX_DMA_THRESHOLD, use single transimit
    //otherwise, use dma
    uint32_t txTransmitRemain;
    UsartTxBuffer* txBuffer;

    if(usartTarget == USART1)
        txBuffer = &usart1TxBuffer;
    else if(usartTarget == USART2)
        txBuffer = &usart2TxBuffer;
    else
        return 1;

    txTransmitRemain =
        (txBuffer->head < txBuffer->tail) ? (txBuffer->head + USART_TX_BUFFER_SIZE - txBuffer->tail) : (txBuffer->head - txBuffer->tail);

    if(txTransmitRemain == 0) return 2;
    if(txTransmitRemain < USART_TX_DMA_THRESHOLD) {
        txBuffer->tailNext = (txBuffer->tail + 1) % USART_TX_BUFFER_SIZE;
        LL_USART_TransmitData8(usartTarget, txBuffer->data[txBuffer->tail]);
    }
    else {
        //Configre DMA
        LL_USART_EnableDMAReq_TX(usartTarget);
        LL_DMA_InitTypeDef usartTxDmaInit;
        usartTxDmaInit.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
        usartTxDmaInit.PeriphOrM2MSrcAddress = (uint32_t)(&(usartTarget->DR));
        usartTxDmaInit.MemoryOrM2MDstAddress = (uint32_t)(txBuffer->data + txBuffer->tail);
        usartTxDmaInit.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
        usartTxDmaInit.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
        usartTxDmaInit.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
        usartTxDmaInit.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
        usartTxDmaInit.Mode = LL_DMA_MODE_NORMAL;
        usartTxDmaInit.Priority = LL_DMA_PRIORITY_MEDIUM;
        if(txBuffer->head < txBuffer->tail) {
            usartTxDmaInit.NbData = USART_TX_BUFFER_SIZE - txBuffer->tail;
            txBuffer->tailNext = 0;
        }
        else {
            usartTxDmaInit.NbData = txBuffer->head - txBuffer->tail;
            txBuffer->tailNext = txBuffer->head;
        }
        uint32_t usartDmaChannel = (usartTarget == USART1) ? LL_DMA_CHANNEL_4 : LL_DMA_CHANNEL_7;
        LL_DMA_Init(DMA1, usartDmaChannel, &usartTxDmaInit);
        //Enable Channl for DMA1
        LL_DMA_EnableChannel(DMA1, usartDmaChannel);
    }
    //Clear USART1 TXE Interrupt flag
    LL_USART_ClearFlag_TC(usartTarget);
    //Enable USART1 Tx
    LL_USART_EnableDirectionTx(usartTarget);
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
    //check if Usart Tx is enable
    //if so, it means dma is running
    if(!(LL_USART_GetTransferDirection(usartTarget) & LL_USART_DIRECTION_TX))UsartTransmitTrigger(usartTarget);
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
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
        usart1TxBuffer.tail = usart1TxBuffer.tailNext;
        if(usart1TxBuffer.tail == usart1TxBuffer.head) {
            //Transmit complete
            LL_USART_DisableDMAReq_TX(USART1);
            LL_USART_DisableDirectionTx(USART1);
        }
        else {
            UsartTransmitTrigger(USART1);
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
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_7);
        usart2TxBuffer.tail = usart2TxBuffer.tailNext;
        if(usart2TxBuffer.tail == usart2TxBuffer.head) {
            //Transmit complete
            LL_USART_DisableDMAReq_TX(USART2);
            LL_USART_DisableDirectionTx(USART2);
        }
        else {
            UsartTransmitTrigger(USART2);
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
