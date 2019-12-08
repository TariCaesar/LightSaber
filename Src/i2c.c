#include "i2c.h"

static struct{
    uint8_t deviceAddr;
    uint8_t* addr;
    uint32_t size;
    int32_t direction;
}i2cFastRequest;

static I2C_TASK i2cTask;

static struct{
    I2C_TASK* task;
    uint32_t taskNum;
    uint32_t taskCnt;
    int32_t active;
}i2cTaskWrap;

int32_t I2cInit(){
    //check the GPIOB clock status
    //if not open, open it
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOB)) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }

    //GPIO AF configuration
    //I2C2 SCL is PB10, I2C2 SDA is PB11, configure into af output
    LL_GPIO_InitTypeDef i2c2GpioInit;
    i2c2GpioInit.Pin = LL_GPIO_PIN_10 | LL_GPIO_PIN_11;
    i2c2GpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
    i2c2GpioInit.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    i2c2GpioInit.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    i2c2GpioInit.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOB, &i2c2GpioInit);

    //enable i2c2 clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

    LL_I2C_InitTypeDef i2c2Init;
    LL_I2C_StructInit(&i2c2Init);
    i2c2Init.ClockSpeed = 200000u;
    i2c2Init.PeripheralMode = LL_I2C_MODE_I2C;
    i2c2Init.OwnAddress1 = 0x52;
    i2c2Init.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    i2c2Init.TypeAcknowledge = LL_I2C_ACK;
    LL_I2C_Init(I2C2, &i2c2Init);

    //Set priority but don't enable interrupt as we'll enable it in fast R/W
    NVIC_SetPriority(I2C2_EV_IRQn, NVIC_EncodePriority(2, 2, 0));
    NVIC_EnableIRQ(I2C2_EV_IRQn);

    LL_I2C_Enable(I2C2);

    UsartSetMystdioHandler(USART2);
    MyPrintf("I2C initialization succeed!\n");
    return 0;
}

uint8_t I2cWriteByte(uint8_t deviceAddr, uint8_t data){
    if(LL_I2C_IsActiveFlag_BUSY(I2C2))return 0;

    //Generate Start Signal, and wait for it complete
    LL_I2C_GenerateStartCondition(I2C2);
    while(!LL_I2C_IsActiveFlag_SB(I2C2))continue;

    LL_I2C_TransmitData8(I2C2, (deviceAddr << 1) & I2C_REQUEST_WRITE);
    while(!LL_I2C_IsActiveFlag_ADDR(I2C2))continue;
    LL_I2C_ClearFlag_ADDR(I2C2);

    while(!LL_I2C_IsActiveFlag_TXE(I2C2))continue;
    LL_I2C_TransmitData8(I2C2, data);

    //wait until the last transfer complete
    while(!LL_I2C_IsActiveFlag_TXE(I2C2) || !LL_I2C_IsActiveFlag_BTF(I2C2))continue;

    LL_I2C_GenerateStopCondition(I2C2);
    //Wait until bus release
    while(LL_I2C_IsActiveFlag_BUSY(I2C2))continue;

    return data;
}

uint16_t I2cWriteHalfWord(uint8_t deviceAddr, uint16_t data){
    if(LL_I2C_IsActiveFlag_BUSY(I2C2))return 0;

    //Generate Start Signal, and wait for it complete
    LL_I2C_GenerateStartCondition(I2C2);
    while(!LL_I2C_IsActiveFlag_SB(I2C2))continue;

    LL_I2C_TransmitData8(I2C2, (deviceAddr << 1) & I2C_REQUEST_WRITE);
    while(!LL_I2C_IsActiveFlag_ADDR(I2C2))continue;
    LL_I2C_ClearFlag_ADDR(I2C2);

    while(!LL_I2C_IsActiveFlag_TXE(I2C2))continue;
    LL_I2C_TransmitData8(I2C2, (uint8_t)((data >> 8) & 0xff));

    while(!LL_I2C_IsActiveFlag_TXE(I2C2))continue;
    LL_I2C_TransmitData8(I2C2, (uint8_t)(data & 0xff));

    //wait until the last transfer complete
    while(!LL_I2C_IsActiveFlag_TXE(I2C2) || !LL_I2C_IsActiveFlag_BTF(I2C2))continue;

    LL_I2C_GenerateStopCondition(I2C2);
    //Wait until bus release
    while(LL_I2C_IsActiveFlag_BUSY(I2C2))continue;

    return data;
}

uint8_t I2cReadByte(uint8_t deviceAddr){
    if(LL_I2C_IsActiveFlag_BUSY(I2C2))return 0;

    //Generate Start Signal, and wait for it complete
    LL_I2C_GenerateStartCondition(I2C2);
    while(!LL_I2C_IsActiveFlag_SB(I2C2))continue;

    //Transfer device Addr for read, and wait for it complete
    LL_I2C_TransmitData8(I2C2, (deviceAddr << 1) | I2C_REQUEST_READ);
    while(!LL_I2C_IsActiveFlag_ADDR(I2C2))continue;
    LL_I2C_AcknowledgeNextData(I2C2, LL_I2C_NACK);
    LL_I2C_ClearFlag_ADDR(I2C2);

    LL_I2C_GenerateStopCondition(I2C2);
    while(!LL_I2C_IsActiveFlag_RXNE(I2C2))continue;
    uint8_t data = LL_I2C_ReceiveData8(I2C2);

    //Wait until bus release
    while(LL_I2C_IsActiveFlag_BUSY(I2C2))continue;

    return data;
}

int32_t I2cReadByteIT(uint8_t deviceAddr, uint8_t* addrDst){
    if(LL_I2C_IsActiveFlag_BUSY(I2C2))return 1;
    i2cTask.deviceAddr = deviceAddr;
    i2cTask.addr = addrDst;
    i2cTask.direction = I2C_REQUEST_READ;

    LL_I2C_EnableIT_EVT(I2C2);
    LL_I2C_EnableIT_BUF(I2C2);
    LL_I2C_GenerateStartCondition(I2C2);
    return 0;
}

int32_t I2cWriteByteIT(uint8_t deviceAddr, uint8_t* addrSrc){
    if(LL_I2C_IsActiveFlag_BUSY(I2C2))return 1;
    i2cTask.deviceAddr = deviceAddr;
    i2cTask.addr = addrSrc;
    i2cTask.direction = I2C_REQUEST_WRITE;

    LL_I2C_EnableIT_EVT(I2C2);
    LL_I2C_EnableIT_BUF(I2C2);
    LL_I2C_GenerateStartCondition(I2C2);
    return 0;
}

int32_t I2cTransferWrap(I2C_TASK* task, uint32_t taskNum){
    if(LL_I2C_IsActiveFlag_BUSY(I2C2))return 1;
    //disable i2c last dma for continous i2c transfer
    if(taskNum > 1)LL_I2C_AcknowledgeNextData(I2C2, LL_I2C_ACK);
    i2cTaskWrap.task = task;
    i2cTaskWrap.taskNum = taskNum;
    i2cTaskWrap.taskCnt = 0;
    i2cTaskWrap.active = 1;
    if(i2cTaskWrap.task[i2cTaskWrap.taskCnt].direction == I2C_REQUEST_WRITE){
        I2cWriteByteIT(i2cTaskWrap.task[i2cTaskWrap.taskCnt].deviceAddr, i2cTaskWrap.task[i2cTaskWrap.taskCnt].addr);
    }
    else{
        I2cReadByteIT(i2cTaskWrap.task[i2cTaskWrap.taskCnt].deviceAddr, i2cTaskWrap.task[i2cTaskWrap.taskCnt].addr);
    }
    return 0;
}

void I2C2_EV_IRQHandler(){
    if(LL_I2C_IsActiveFlag_SB(I2C2)){
        if(i2cTask.direction == I2C_REQUEST_WRITE){
            LL_I2C_TransmitData8(I2C2, (i2cFastRequest.deviceAddr << 1) & I2C_REQUEST_WRITE);
        }
        else{
            LL_I2C_TransmitData8(I2C2, (i2cFastRequest.deviceAddr << 1) | I2C_REQUEST_READ);
        }
    }
    else if(LL_I2C_IsActiveFlag_ADDR(I2C2)){
        LL_I2C_ClearFlag_ADDR(I2C2);
        if(i2cTaskWrap.active){
            if(i2cTaskWrap.taskCnt + 1 == i2cTaskWrap.taskNum){
                LL_I2C_AcknowledgeNextData(I2C2, LL_I2C_NACK);
            }
            else{
                LL_I2C_AcknowledgeNextData(I2C2, LL_I2C_ACK);
            }
            if(i2cTask.direction == I2C_REQUEST_WRITE)LL_I2C_TransmitData8(I2C2, *(i2cTask.addr));
        }
        else{
            LL_I2C_AcknowledgeNextData(I2C2, LL_I2C_NACK);
            if(i2cTask.direction == I2C_REQUEST_WRITE){
                LL_I2C_TransmitData8(I2C2, *(i2cTask.addr));
            }
            else{
                LL_I2C_GenerateStopCondition(I2C2);
            }
        }
    }
    else if(LL_I2C_IsActiveFlag_RXNE(I2C2)){
        *(i2cTask.addr) = LL_I2C_ReceiveData8(I2C2);
        if(i2cTaskWrap.active){
            i2cTaskWrap.taskCnt += 1;
            if(i2cTaskWrap.taskCnt == i2cTaskWrap.taskNum){
                i2cTaskWrap.active = 0;
                LL_I2C_DisableIT_EVT(I2C2);
                LL_I2C_DisableIT_BUF(I2C2);
            }
            else{
                if(i2cTaskWrap.task[i2cTaskWrap.taskCnt].direction == I2C_REQUEST_WRITE){
                    I2cWriteByteIT(i2cTaskWrap.task[i2cTaskWrap.taskCnt].deviceAddr, i2cTaskWrap.task[i2cTaskWrap.taskCnt].addr);
                }
                else{
                    I2cReadByteIT(i2cTaskWrap.task[i2cTaskWrap.taskCnt].deviceAddr, i2cTaskWrap.task[i2cTaskWrap.taskCnt].addr);
                }
            }
        }
        else{
            LL_I2C_DisableIT_EVT(I2C2);
            LL_I2C_DisableIT_BUF(I2C2);
        }
    }
    else if(LL_I2C_IsActiveFlag_TXE(I2C2)){
        if(i2cTaskWrap.active){
            i2cTaskWrap.taskCnt += 1;
            if(i2cTaskWrap.taskCnt == i2cTaskWrap.taskNum){
                i2cTaskWrap.active = 0;
                LL_I2C_GenerateStopCondition(I2C2);
                LL_I2C_DisableIT_EVT(I2C2);
                LL_I2C_DisableIT_BUF(I2C2);
            }
            else{
                if(i2cTaskWrap.task[i2cTaskWrap.taskCnt].direction == I2C_REQUEST_WRITE){
                    I2cWriteByteIT(i2cTaskWrap.task[i2cTaskWrap.taskCnt].deviceAddr, i2cTaskWrap.task[i2cTaskWrap.taskCnt].addr);
                }
                else{
                    I2cReadByteIT(i2cTaskWrap.task[i2cTaskWrap.taskCnt].deviceAddr, i2cTaskWrap.task[i2cTaskWrap.taskCnt].addr);
                }
            }
        }
        else{
            LL_I2C_GenerateStopCondition(I2C2);
            LL_I2C_DisableIT_EVT(I2C2);
            LL_I2C_DisableIT_BUF(I2C2);
        } 
    }
    return;
}
