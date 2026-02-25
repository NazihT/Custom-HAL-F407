#include "stm32f407xx.h"


typedef enum
{
    I2C_Standard_Mode,
    I2C_Fast_Mode

}i2c_Speed;


typedef struct
{
    i2c_Speed  speed_mode;
    uint16_t   frequency;

} I2C_Config_t ;

typedef enum
{
    NACK,
    ACK

} ACK_Status;


void I2C_Init(I2C_TypeDef *i2cx , I2C_Config_t *Config );
ACK_Status I2C_Transmit(I2C_TypeDef *i2cx , uint8_t addr , uint8_t *data , uint8_t size);
void I2C_SetSpeed(I2C_TypeDef *i2cx , i2c_Speed Speed , uint16_t frequency);
uint16_t get_CCR_SM(uint32_t pclk, uint32_t scl_freq) ;
uint16_t get_CCR_FM(uint32_t pclk, uint32_t scl_freq) ;

ACK_Status I2C_Recieve(I2C_TypeDef* i2cx, uint8_t addr , uint8_t size , uint8_t* buffer);


void I2C_EnableClock(I2C_TypeDef *i2cx);