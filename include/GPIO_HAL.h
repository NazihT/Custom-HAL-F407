#include "stm32f407xx.h"

typedef enum {
    INPUT  = 0,
    OUTPUT = 1,
    AF     = 2,
    ANALOG = 3
} GPIO_Mode_t;
typedef enum {

    Push_Pull ,
    Open_Drain

}OutputType;

typedef enum {

    Pull_Up=0,
    Pull_Down=1
    
}Pup_Pdown;


typedef enum {
    low_speed ,
    medium_speed,
    high_speed,
    very_high_speed

} Speed;

typedef enum {
    AF0,
    AF1,
    AF2,
    AF3,
    AF4,
    AF5,
    AF6,
    AF7,
    AF8,
    AF9,
    AF10,
    AF11,
    AF12,
    AF13,
    AF14,
    AF15
} AlternateFunction_t;


typedef struct {
    uint8_t Pin;
    GPIO_Mode_t Mode;
    OutputType Type; 
    Speed Speed;
    Pup_Pdown Pull;
    AlternateFunction_t AFx;
    
} GPIO_Config_t;

void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_Config_t *InitStruct);
void GPIO_SetAFmode(GPIO_TypeDef *GPIOx,uint8_t pin,AlternateFunction_t AFx);




void GPIO_WritePin(GPIO_TypeDef *GPIOx , uint8_t PIN , uint8_t State);
void GPIO_TogglePin(GPIO_TypeDef *GPIOx , uint8_t PIN);
uint8_t GPIO_ReadPin (GPIO_TypeDef *GPIOx , uint8_t PIN);



void GPIO_PupPdown (GPIO_TypeDef *GPIOx,uint8_t PIN , Pup_Pdown resistor );



void GPIO_Output_Type (GPIO_TypeDef *GPIOx , uint8_t PIN , OutputType Type );



void GPIO_Output_Speed (GPIO_TypeDef *GPIOx , uint8_t PIN , Speed speed);
void GPIO_EnableClock(GPIO_TypeDef *GPIOx);

