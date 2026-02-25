#include "I2C_HAL.h"
#include "RCC_HAL.h"


void I2C_Init(I2C_TypeDef *i2cx , I2C_Config_t *Config )
{  I2C_EnableClock(i2cx);
    I2C_SetSpeed(i2cx,Config->speed_mode,Config->frequency);
        i2cx->TRISE = (PCLK1_FREQ / 1000000) + 1;

    i2cx->CR1 |=(1<<I2C_CR1_PE_Pos);
    
}

ACK_Status I2C_Transmit(I2C_TypeDef *i2cx, uint8_t addr, uint8_t *data, uint8_t size)
{
    uint32_t timeout;
    
    // Wait for bus not busy
    timeout = 160000;
    while ((i2cx->SR2 & I2C_SR2_BUSY) && timeout--);
    if (!timeout) return NACK;
    
    i2cx->CR1 |= I2C_CR1_START;
    
    // Wait for Start Bit
    timeout = 160000;
    while (!(i2cx->SR1 & I2C_SR1_SB) && timeout--);
    if (!timeout) return NACK;
    
    i2cx->DR = (addr << 1) | 0; // write
    
    timeout = 160000;
    while (!(i2cx->SR1 & I2C_SR1_ADDR) && !(i2cx->SR1 & I2C_SR1_AF) && timeout--);
    
    if (i2cx->SR1 & I2C_SR1_AF)
    {
        (void)i2cx->SR1;
        i2cx->SR1 &= ~I2C_SR1_AF;  // Clear AF
        i2cx->CR1 |= I2C_CR1_STOP;
        return NACK;
    }
    
    if (!timeout)
    {
        i2cx->CR1 |= I2C_CR1_STOP;
        return NACK;
    }
    
    // Clear ADDR flag
    (void)i2cx->SR1;
    (void)i2cx->SR2;
    
    // Special case: size = 0 (just address check)
    if (size == 0)
    {
        i2cx->CR1 |= I2C_CR1_STOP;
        return ACK;
    }
    
    // Send data bytes
    for (uint8_t i = 0; i < size; i++)
    {
        while (!(i2cx->SR1 & I2C_SR1_TXE) && 
               !(i2cx->SR1 & I2C_SR1_AF));
        
        if (i2cx->SR1 & I2C_SR1_AF)
        {
            (void)i2cx->SR1;
            i2cx->SR1 &= ~I2C_SR1_AF;
            i2cx->CR1 |= I2C_CR1_STOP;
            return NACK;
        }
        
        i2cx->DR = data[i];
    }
    
    // Wait for last byte transmission complete
    while (!(i2cx->SR1 & I2C_SR1_BTF) && 
           !(i2cx->SR1 & I2C_SR1_AF));
    
    if (i2cx->SR1 & I2C_SR1_AF)
    {
        (void)i2cx->SR1;
        i2cx->SR1 &= ~I2C_SR1_AF;
        i2cx->CR1 |= I2C_CR1_STOP;
        return NACK;
    }
    
    i2cx->CR1 |= I2C_CR1_STOP;
    return ACK;
}

ACK_Status I2C_Receive(I2C_TypeDef* i2cx, uint8_t addr, uint8_t size, uint8_t* buffer)
{
    uint32_t timeout;
    
    // Wait for bus not busy
    timeout = 160000;
    while ((i2cx->SR2 & I2C_SR2_BUSY) && timeout--);
    if (!timeout) return NACK;
    
    i2cx->CR1 |= I2C_CR1_START;
    
    // Wait for Start Bit
    timeout = 160000;
    while (!(i2cx->SR1 & I2C_SR1_SB) && timeout--);
    if (!timeout) return NACK;
    
    i2cx->DR = (addr << 1) | 0x1; // read
    
    // Wait for ADDR or AF
    timeout = 160000;
    while (!(i2cx->SR1 & I2C_SR1_ADDR) && 
           !(i2cx->SR1 & I2C_SR1_AF) && 
           timeout--);
    
    if (i2cx->SR1 & I2C_SR1_AF)
    {
        (void)i2cx->SR1;
        i2cx->SR1 &= ~I2C_SR1_AF;
        i2cx->CR1 |= I2C_CR1_STOP;
        return NACK;
    }
    
    if (!timeout)
    {
        i2cx->CR1 |= I2C_CR1_STOP;
        return NACK;
    }
    
    // Clear ADDR flag
    (void)i2cx->SR1;
    (void)i2cx->SR2;
    i2cx->CR1 |= I2C_CR1_ACK;

    
    // Receive bytes
    for (int index = 0; index < size; index++)
    {
        // Set NACK on last byte
        if (index == size - 1)
        {
            i2cx->CR1 &= ~I2C_CR1_ACK;  // NACK for last byte
        }
        
        // Wait for data with FRESH timeout
        timeout = 160000;
        while (!(i2cx->SR1 & I2C_SR1_RXNE) && timeout--);
        
        if (!timeout) 
        {
            i2cx->CR1 |= I2C_CR1_STOP;
            return NACK;
        }
        
        buffer[index] = i2cx->DR;
    }
    
    i2cx->CR1 |= I2C_CR1_STOP;
    return ACK;
}


void I2C_SetSpeed(I2C_TypeDef *i2cx , i2c_Speed Speed_Mode , uint16_t frequency)
{
    if (Speed_Mode == I2C_Standard_Mode)
    {
        uint16_t CCR_value = get_CCR_SM(PCLK1_FREQ, frequency);
        i2cx->CCR &= ~(1 << I2C_CCR_FS_Pos);
        i2cx->CCR &= ~(0xFFF);
        i2cx->CCR |= (CCR_value << 0);
    }
    else if (Speed_Mode == I2C_Fast_Mode)
    {
        uint16_t CCR_value = get_CCR_FM(PCLK1_FREQ, frequency);
        i2cx->CCR |= (1 << I2C_CCR_FS_Pos);
        i2cx->CCR &= ~(0xFFF);
        i2cx->CCR |= (CCR_value << 0);
    }
}

uint16_t get_CCR_SM(uint32_t pclk, uint32_t scl_freq) {
    return pclk / (2 * scl_freq * 1000); 
}

uint16_t get_CCR_FM(uint32_t pclk, uint32_t scl_freq) {
    return pclk / (3 * scl_freq * 1000); 
}

void I2C_EnableClock(I2C_TypeDef *i2cx)
{
    if (i2cx == I2C1)
    {
        RCC->APB1ENR |= (1 << RCC_APB1ENR_I2C1EN_Pos);
    }
    if (i2cx == I2C2)
    {
        RCC->APB1ENR |= (1 << RCC_APB1ENR_I2C2EN_Pos);
    }
    if (i2cx == I2C3)
    {
        RCC->APB1ENR |= (1 << RCC_APB1ENR_I2C3EN_Pos);
    }
}
