#include "GPIO_HAL.h"

void GPIO_WritePin(GPIO_TypeDef *GPIOx , uint8_t PIN , uint8_t State)
{if (State==1)
   {
    GPIOx->BSRR = (1<<PIN);
   }
   else
   {
    GPIOx->BSRR =(1<<(PIN+16));
   }
}

 void GPIO_PinMode(GPIO_TypeDef *GPIOx, uint8_t PIN , GPIO_Mode_t MODE ) 
 { 
    uint8_t position = PIN * 2 ; 
    GPIOx->MODER &=~(3<<position); 
    switch (MODE) 
    { 
        case OUTPUT: GPIOx->MODER |=(1<<position); break; 
        case INPUT : GPIOx->MODER &=~(3<<position); break; 
        case AF : GPIOx->MODER |= (2<<position); break;
        case ANALOG: GPIOx->MODER |=(3<<position); break; 
    } 
}

void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_Config_t *InitStruct) 
{
    GPIO_EnableClock(GPIOx);
    GPIO_PinMode(GPIOx, InitStruct->Pin, InitStruct->Mode);
    if(InitStruct->Mode==OUTPUT)
    {
      GPIO_Output_Type(GPIOx, InitStruct->Pin, InitStruct->Type);
      GPIO_Output_Speed(GPIOx, InitStruct->Pin, InitStruct->Speed);
    }
    else if (InitStruct->Mode==INPUT)
    {
      GPIO_PupPdown(GPIOx, InitStruct->Pin, InitStruct->Pull);
    }
    else if (InitStruct->Mode==AF)
    {
        GPIO_Output_Type(GPIOx, InitStruct->Pin, InitStruct->Type);
        GPIO_Output_Speed(GPIOx, InitStruct->Pin, InitStruct->Speed);
        GPIO_PupPdown(GPIOx, InitStruct->Pin, InitStruct->Pull);
        GPIO_SetAFmode(GPIOx, InitStruct->Pin, InitStruct->AFx);   
     }
}
void GPIO_SetAFmode(GPIO_TypeDef *GPIOx,uint8_t pin ,AlternateFunction_t AFx)
{
    uint8_t pos = (pin % 8)*4;
    if (pin<8)
    {
            GPIOx->AFR[0] = (GPIOx->AFR[0] &~ (0xF<<pos) ) | (AFx<<pos);
    }
    else
    {
        GPIOx->AFR[1] = (GPIOx->AFR[1] &~ (0xF<<pos) ) | (AFx<<pos);

    }
}



void GPIO_TogglePin(GPIO_TypeDef *GPIOx , uint8_t PIN)
{
    GPIOx->ODR ^=(1<<PIN);

}

uint8_t GPIO_ReadPin (GPIO_TypeDef *GPIOx , uint8_t PIN)
{
    uint8_t state;
    state = (GPIOx->IDR & (1<<PIN)) >> PIN ;
    return state;
}

void GPIO_PupPdown (GPIO_TypeDef *GPIOx,uint8_t PIN , Pup_Pdown resistor )
{
    switch (resistor)
    {
    case Pull_Up:
        GPIOx->PUPDR &=~(3<<PIN*2);
        GPIOx->PUPDR |=(1<<PIN*2);

        break;
    
    case Pull_Down:
        GPIOx->PUPDR &=~(3<<PIN*2);
        GPIOx->PUPDR |=(2<<PIN*2);
  
        break;
    }
}

void GPIO_Output_Type (GPIO_TypeDef *GPIOx , uint8_t PIN , OutputType Type )
{
    switch (Type)
    {
    case Push_Pull:
        GPIOx->OTYPER &=~(1<<PIN);
        break;
    
    case Open_Drain:
        GPIOx->OTYPER |=(1<<PIN);
        break;
    }
}

void GPIO_Output_Speed (GPIO_TypeDef *GPIOx , uint8_t PIN , Speed speed)
{
    switch (speed)
    {
    case low_speed:
        GPIOx->OSPEEDR &=~(3<<PIN*2);
        break;
    
    case medium_speed:
        GPIOx->OSPEEDR &=~(3<<PIN*2);
        GPIOx->OSPEEDR |=(1<<PIN*2);
        break;
    
    case high_speed :
        GPIOx->OSPEEDR &=~(3<<PIN*2);
        GPIOx->OSPEEDR |=(2<<PIN*2);
        break; 
    case very_high_speed:
        GPIOx->OSPEEDR &=~(3<<PIN*2);
        GPIOx->OSPEEDR |=(3<<PIN*2);
        break;
    }
}

void GPIO_EnableClock(GPIO_TypeDef *GPIOx)
{
    if (GPIOx == GPIOA) RCC->AHB1ENR |= (1 << 0);
else if (GPIOx == GPIOB) RCC->AHB1ENR |= (1 << 1);
else if (GPIOx == GPIOC) RCC->AHB1ENR |= (1 << 2);
else if (GPIOx == GPIOD) RCC->AHB1ENR |= (1 << 3);
else if (GPIOx == GPIOE) RCC->AHB1ENR |= (1 << 4);
else if (GPIOx == GPIOF) RCC->AHB1ENR |= (1 << 5);
else if (GPIOx == GPIOG) RCC->AHB1ENR |= (1 << 6);
else if (GPIOx == GPIOH) RCC->AHB1ENR |= (1 << 7);
else if (GPIOx == GPIOI) RCC->AHB1ENR |= (1 << 8);

}


