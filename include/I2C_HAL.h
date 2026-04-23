#ifndef I2C_HAL_H
#define I2C_HAL_H

#include "stm32f407xx.h"

/* ---- assert_param ---- */
#ifdef USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t *file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

/* ---- validation macros ---- */
#define IS_I2C(i2c)        ((i2c) == I2C1 || (i2c) == I2C2 || (i2c) == I2C3)
#define IS_I2C_SPEED(s)    ((s) == I2C_STANDARD_MODE || (s) == I2C_FAST_MODE)

#define PCLK1_FREQ         16000000U

typedef enum {
    I2C_STANDARD_MODE,
    I2C_FAST_MODE,
} I2C_Speed;

typedef struct {
    I2C_Speed speed_mode;
    uint16_t  frequency;       // in kHz  (e.g. 100 = 100kHz, 400 = 400kHz)
} I2C_Config_t;

typedef enum {
    NACK,
    ACK,
} ACK_Status;

void       I2C_Init(I2C_TypeDef *i2cx, I2C_Config_t *cfg);
void       I2C_EnableClock(I2C_TypeDef *i2cx);
void       I2C_SetSpeed(I2C_TypeDef *i2cx, I2C_Speed speed_mode, uint16_t frequency);
ACK_Status I2C_Transmit(I2C_TypeDef *i2cx, uint8_t addr, uint8_t *data, uint8_t size);
ACK_Status I2C_Receive(I2C_TypeDef *i2cx, uint8_t addr, uint8_t size, uint8_t *buffer);

#endif