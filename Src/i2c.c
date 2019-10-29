#include "i2c.h"

int32_t I2cInit(){
    //check the GPIOB clock status
    //if not open, open it
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOB)){
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }

    //GPIO AF configuration
    //I2C1 SCL is PB6, I2C2 SDA is PB7, configure into af output
    LL_GPIO_InitTypeDef i2c1GpioInit;
    i2c1GpioInit.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    i2c1GpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
    i2c1GpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    i2c1GpioInit.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
    i2c1GpioInit.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOB, &i2c1GpioInit); 

    //enable i2c1 clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    LL_I2C_InitTypeDef i2c1Init;
    i2c1Init.ClockSpeed = LL_I2C_CLOCK_SPEED_STANDARD_MODE;
    return 0;
}