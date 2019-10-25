#include "usart.h"

static UsartRxBuffer rxBuffer;
static UsartTxBuffer txBuffer;

int UsartInit(){
    //check the GPIOA clock status
    //if not open, open it
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOA)){
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    }
    //GPIO AF configuration
    //USART1 Tx is PA9, configure into af output
    LL_GPIO_InitTypeDef usart1GpioInit;
    usart1GpioInit.Pin = LL_GPIO_PIN_9;
    usart1GpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
    usart1GpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    usart1GpioInit.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
    usart1GpioInit.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOA, &usart1GpioInit); 

    //USART1 Rx is PA10, configure into float input
    usart1GpioInit.Pin = LL_GPIO_PIN_10;
    usart1GpioInit.Mode = LL_GPIO_MODE_FLOATING;
    LL_GPIO_Init(GPIOA, &usart1GpioInit); 

    //Enable usart1 clock
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

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
    //Enable Usart1 TxDMA
    LL_USART_EnableDMAReq_TX(USART1);

    //Set usart into asysn mode
    LL_USART_ConfigAsyncMode(USART1);

    //Init usart rx buffer
    rxBuffer.head = 0;
    rxBuffer.tail = 0;

    //Init usart tx buffer
    txBuffer.head = 0;
    txBuffer.tail = 0;
    txBuffer.tailNext= 0;

    //Init rx interrupt
    LL_USART_ClearFlag_RXNE(USART1);
    LL_USART_EnableIT_RXNE(USART1);
    LL_USART_ClearFlag_TC(USART1);
    LL_USART_EnableIT_TC(USART1);
    //Enable USART1 in NVIC
    NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(2, 2, 0));
    NVIC_EnableIRQ(USART1_IRQn);


    //Enable Usart1 
    LL_USART_Enable(USART1);

    return 0;
}

int UsartReadBuffer(uint8_t *addr_dst){
    //judge if the cycle buffer is empty
    if(rxBuffer.head == rxBuffer.tail)return 1;
    *addr_dst = rxBuffer.data[rxBuffer.tail];
    rxBuffer.tail = (rxBuffer.tail + 1) % USART_RX_BUFFER_SIZE;
    return 0;
}

static int UsartTransmit(){
    //if buffer remain less than USART_TX_DMA_THRESHOLD, use single transimit
    //otherwise, use dma
    uint32_t txTransmitRemain =
    (txBuffer.head < txBuffer.tail)? 
    (txBuffer.head + USART_TX_BUFFER_SIZE - txBuffer.tail):
    (txBuffer.head - txBuffer.tail);

    if(txTransmitRemain == 0)return 1;
    if(txTransmitRemain < USART_TX_DMA_THRESHOLD){
        txBuffer.tailNext = (txBuffer.head + 1) % USART_TX_BUFFER_SIZE;
        LL_USART_TransmitData8(USART1, txBuffer.data[txBuffer.tail]);
    }
    else{
        //Configre DMA1 Channel4
        LL_DMA_InitTypeDef usart1TxDmaInit;
        usart1TxDmaInit.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
        usart1TxDmaInit.PeriphOrM2MSrcAddress = (uint32_t)(&(USART1->DR));
        usart1TxDmaInit.MemoryOrM2MDstAddress = (uint32_t)(txBuffer.data + txBuffer.tail);
        usart1TxDmaInit.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
        usart1TxDmaInit.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
        usart1TxDmaInit.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
        usart1TxDmaInit.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
        usart1TxDmaInit.Mode = LL_DMA_MODE_NORMAL;
        usart1TxDmaInit.Priority = LL_DMA_PRIORITY_MEDIUM; 
        if(txBuffer.head < rxBuffer.tail){
            usart1TxDmaInit.NbData = USART_TX_BUFFER_SIZE - txBuffer.tail;
            txBuffer.tailNext = 0;
        }
        else{
            usart1TxDmaInit.NbData = txBuffer.head -txBuffer.tail;
            txBuffer.tailNext = txBuffer.head;
        }
        LL_DMA_Init(DMA1, LL_DMA_CHANNEL_4, &usart1TxDmaInit);
        //Enable Channl4 for DMA1
        LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
        //Clear USART1 TC Interrupt flag
    }
    //Clear USART1 TC Interrupt flag
    LL_USART_ClearFlag_TC(USART1);
    //Enable USART1 Tx
    LL_USART_EnableDirectionTx(USART1);
    return 0;
}

int UsartSendChar(char ch){
    uint32_t head_next = (txBuffer.head + 1) % USART_TX_BUFFER_SIZE;
    //if txBuffer is full, return error
    if(head_next == txBuffer.tail)return 1;
    txBuffer.data[txBuffer.head] = ch;
    txBuffer.head = head_next;
    //check if Usart Tx is enable
    //if so, it means dma is running
    if(LL_USART_GetTransferDirection(USART1) != LL_USART_DIRECTION_TX_RX)UsartTransmit();
    return 0;
}

int UsartSendData(uint8_t* addr, uint32_t size){
    uint32_t i;
    //check if buffer is large enough
    uint32_t txBufferRemain = 
    (txBuffer.head < txBuffer.tail)? 
    (txBuffer.tail - txBuffer.head - 1):
    (USART_TX_BUFFER_SIZE - txBuffer.head + txBuffer.tail - 1);
    if(txBufferRemain < size)return 1;
    for(i = 0; i < size; ++i){
        txBuffer.data[txBuffer.head] = addr[i];
        txBuffer.head = (txBuffer.head + 1) % USART_TX_BUFFER_SIZE;
    }
    if(LL_USART_GetTransferDirection(USART1) != LL_USART_DIRECTION_TX_RX)UsartTransmit();
    return 0;
}


void USART1_IRQHandler(){
    if(LL_USART_IsActiveFlag_TC(USART1)){
        //clear the interrupt flag
        LL_USART_ClearFlag_TC(USART1);
        //Disable DMA for reuse
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
        txBuffer.tail = txBuffer.tailNext;
        if(txBuffer.tail == txBuffer.head){
            //Transmit complete
            LL_USART_DisableDirectionTx(USART1);
        }
        else{
            UsartTransmit();
        }
    }
    else if(LL_USART_IsActiveFlag_RXNE(USART1)){
        rxBuffer.data[rxBuffer.head] = LL_USART_ReceiveData8(USART1);
        rxBuffer.head = (rxBuffer.head + 1) % USART_RX_BUFFER_SIZE;
        //if rxBuffer is full, throw the oldest data
        if(rxBuffer.head == rxBuffer.tail){
            rxBuffer.tail = (rxBuffer.tail + 1) % USART_RX_BUFFER_SIZE;
        }
        //clear the interrupt flag
        LL_USART_ClearFlag_RXNE(USART1);
    }
}