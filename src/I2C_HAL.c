#include "I2C_HAL.h"

/* ---- internal helpers ---- */
static uint16_t I2C_CalcCCR_SM(uint32_t pclk, uint32_t scl_khz)
{
    return (uint16_t)(pclk / (2U * scl_khz * 1000U));
}

static uint16_t I2C_CalcCCR_FM(uint32_t pclk, uint32_t scl_khz)
{
    return (uint16_t)(pclk / (3U * scl_khz * 1000U));
}

/* ------------------------------------------------------------------ */

void I2C_EnableClock(I2C_TypeDef *i2cx)
{
    assert_param(IS_I2C(i2cx));

    if      (i2cx == I2C1) RCC->APB1ENR |= (1 << RCC_APB1ENR_I2C1EN_Pos);
    else if (i2cx == I2C2) RCC->APB1ENR |= (1 << RCC_APB1ENR_I2C2EN_Pos);
    else                   RCC->APB1ENR |= (1 << RCC_APB1ENR_I2C3EN_Pos);
}

void I2C_SetSpeed(I2C_TypeDef *i2cx, I2C_Speed speed_mode, uint16_t frequency)
{
    assert_param(IS_I2C(i2cx));
    assert_param(IS_I2C_SPEED(speed_mode));

    i2cx->CCR &= ~(0xFFF);

    if (speed_mode == I2C_STANDARD_MODE) {
        i2cx->CCR &= ~(1 << I2C_CCR_FS_Pos);
        i2cx->CCR |=  I2C_CalcCCR_SM(PCLK1_FREQ, frequency);
        i2cx->TRISE = (PCLK1_FREQ / 1000000U) + 1U;            // Trise = 1000ns
    } else {
        i2cx->CCR |= (1 << I2C_CCR_FS_Pos);
        i2cx->CCR |=  I2C_CalcCCR_FM(PCLK1_FREQ, frequency);
        i2cx->TRISE = ((PCLK1_FREQ / 1000000U) * 3U) / 10U + 1U; // Trise = 300ns
    }
}

void I2C_Init(I2C_TypeDef *i2cx, I2C_Config_t *cfg)
{
    assert_param(IS_I2C(i2cx));
    assert_param(cfg != NULL);
    assert_param(IS_I2C_SPEED(cfg->speed_mode));

    I2C_EnableClock(i2cx);

    // CR2 FREQ must be set to APB1 clock in MHz before touching CCR/TRISE
    i2cx->CR2 &= ~(0x3F);
    i2cx->CR2 |=  (PCLK1_FREQ / 1000000U);

    I2C_SetSpeed(i2cx, cfg->speed_mode, cfg->frequency);

    i2cx->CR1 |= (1 << I2C_CR1_PE_Pos);
}

ACK_Status I2C_Transmit(I2C_TypeDef *i2cx, uint8_t addr, uint8_t *data, uint8_t size)
{
    assert_param(IS_I2C(i2cx));

    uint32_t timeout;

    timeout = 160000U;
    while ((i2cx->SR2 & I2C_SR2_BUSY) && timeout--);
    if (!timeout) return NACK;

    i2cx->CR1 |= I2C_CR1_START;

    timeout = 160000U;
    while (!(i2cx->SR1 & I2C_SR1_SB) && timeout--);
    if (!timeout) return NACK;

    i2cx->DR = (uint8_t)((addr << 1) | 0x00);   // write

    timeout = 160000U;
    while (!(i2cx->SR1 & I2C_SR1_ADDR) && !(i2cx->SR1 & I2C_SR1_AF) && timeout--);

    if (i2cx->SR1 & I2C_SR1_AF) {
        i2cx->SR1  &= ~I2C_SR1_AF;
        i2cx->CR1  |=  I2C_CR1_STOP;
        return NACK;
    }
    if (!timeout) {
        i2cx->CR1 |= I2C_CR1_STOP;
        return NACK;
    }

    // clear ADDR
    (void)i2cx->SR1;
    (void)i2cx->SR2;

    if (size == 0) {
        i2cx->CR1 |= I2C_CR1_STOP;
        return ACK;
    }

    for (uint8_t i = 0; i < size; i++) {
        while (!(i2cx->SR1 & I2C_SR1_TXE) && !(i2cx->SR1 & I2C_SR1_AF));

        if (i2cx->SR1 & I2C_SR1_AF) {
            i2cx->SR1 &= ~I2C_SR1_AF;
            i2cx->CR1 |=  I2C_CR1_STOP;
            return NACK;
        }

        i2cx->DR = data[i];
    }

    while (!(i2cx->SR1 & I2C_SR1_BTF) && !(i2cx->SR1 & I2C_SR1_AF));

    if (i2cx->SR1 & I2C_SR1_AF) {
        i2cx->SR1 &= ~I2C_SR1_AF;
        i2cx->CR1 |=  I2C_CR1_STOP;
        return NACK;
    }

    i2cx->CR1 |= I2C_CR1_STOP;
    return ACK;
}

ACK_Status I2C_Receive(I2C_TypeDef *i2cx, uint8_t addr, uint8_t size, uint8_t *buffer)
{
    assert_param(IS_I2C(i2cx));
    assert_param(buffer != NULL);

    uint32_t timeout;

    timeout = 160000U;
    while ((i2cx->SR2 & I2C_SR2_BUSY) && timeout--);
    if (!timeout) return NACK;

    i2cx->CR1 |= I2C_CR1_START;

    timeout = 160000U;
    while (!(i2cx->SR1 & I2C_SR1_SB) && timeout--);
    if (!timeout) return NACK;

    i2cx->DR = (uint8_t)((addr << 1) | 0x01);   // read

    timeout = 160000U;
    while (!(i2cx->SR1 & I2C_SR1_ADDR) && !(i2cx->SR1 & I2C_SR1_AF) && timeout--);

    if (i2cx->SR1 & I2C_SR1_AF) {
        i2cx->SR1 &= ~I2C_SR1_AF;
        i2cx->CR1 |=  I2C_CR1_STOP;
        return NACK;
    }
    if (!timeout) {
        i2cx->CR1 |= I2C_CR1_STOP;
        return NACK;
    }

    // clear ADDR
    (void)i2cx->SR1;
    (void)i2cx->SR2;

    i2cx->CR1 |= I2C_CR1_ACK;

    for (uint8_t i = 0; i < size; i++) {
        if (i == size - 1)
            i2cx->CR1 &= ~I2C_CR1_ACK;     // NACK before reading last byte

        timeout = 160000U;
        while (!(i2cx->SR1 & I2C_SR1_RXNE) && timeout--);
        if (!timeout) {
            i2cx->CR1 |= I2C_CR1_STOP;
            return NACK;
        }

        buffer[i] = (uint8_t)i2cx->DR;
    }

    i2cx->CR1 |= I2C_CR1_STOP;
    return ACK;
}