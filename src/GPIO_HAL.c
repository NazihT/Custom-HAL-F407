#include "GPIO_HAL.h"

__attribute__((weak)) void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;

    // red LED on PD14 
    RCC->AHB1ENR  |=  (1 << 3);
    GPIOD->MODER  |=  (1 << 28);
    GPIOD->BSRR    =  (1 << 14);

    while(1);
}


void GPIO_PinMode(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Mode_t mode)
{
    assert_param(IS_GPIO_PORT(GPIOx));
    assert_param(IS_GPIO_PIN(pin));
    assert_param(IS_GPIO_MODE(mode));

    uint8_t pos = pin * 2;
    GPIOx->MODER &= ~(3 << pos);
    GPIOx->MODER |=  (mode << pos);
}

void GPIO_SetAFmode(GPIO_TypeDef *GPIOx, uint8_t pin, AlternateFunction_t AFx)
{
    assert_param(IS_GPIO_PORT(GPIOx));
    assert_param(IS_GPIO_PIN(pin));
    assert_param(IS_GPIO_AF(AFx));

    uint8_t pos = (pin % 8) * 4;
    uint8_t idx = (pin < 8) ? 0 : 1;

    GPIOx->AFR[idx] = (GPIOx->AFR[idx] & ~(0xF << pos)) | (AFx << pos);
}

void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t state)
{
    assert_param(IS_GPIO_PORT(GPIOx));
    assert_param(IS_GPIO_PIN(pin));
    assert_param(IS_GPIO_STATE(state));

    if (state == 1)
        GPIOx->BSRR = (1 << pin);
    else
        GPIOx->BSRR = (1 << (pin + 16));
}

void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint8_t pin)
{
    assert_param(IS_GPIO_PORT(GPIOx));
    assert_param(IS_GPIO_PIN(pin));

    GPIOx->ODR ^= (1 << pin);
}

uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t pin)
{
    assert_param(IS_GPIO_PORT(GPIOx));
    assert_param(IS_GPIO_PIN(pin));

    return (GPIOx->IDR & (1 << pin)) >> pin;
}

void GPIO_PupPdown(GPIO_TypeDef *GPIOx, uint8_t pin, Pup_Pdown resistor)
{
    assert_param(IS_GPIO_PORT(GPIOx));
    assert_param(IS_GPIO_PIN(pin));
    assert_param(IS_GPIO_PULL(resistor));

    uint8_t pos = pin * 2;
    GPIOx->PUPDR &= ~(3 << pos);
    GPIOx->PUPDR |=  (resistor << pos);
}

void GPIO_Output_Type(GPIO_TypeDef *GPIOx, uint8_t pin, OutputType type)
{
    assert_param(IS_GPIO_PORT(GPIOx));
    assert_param(IS_GPIO_PIN(pin));
    assert_param(IS_GPIO_OTYPE(type));

    GPIOx->OTYPER &= ~(1 << pin);
    GPIOx->OTYPER |=  (type << pin);
}

void GPIO_Output_Speed(GPIO_TypeDef *GPIOx, uint8_t pin, Speed speed)
{
    assert_param(IS_GPIO_PORT(GPIOx));
    assert_param(IS_GPIO_PIN(pin));
    assert_param(IS_GPIO_SPEED(speed));

    uint8_t pos = pin * 2;
    GPIOx->OSPEEDR &= ~(3 << pos);
    GPIOx->OSPEEDR |=  (speed << pos);
}

HAL_Status_t GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_Config_t *cfg)
{
    assert_param(IS_GPIO_PORT(GPIOx));
    assert_param(cfg != NULL);
    assert_param(IS_GPIO_PIN(cfg->Pin));
    assert_param(IS_GPIO_MODE(cfg->Mode));
    if (!IS_GPIO_PORT(GPIOx) || cfg == NULL)  return HAL_ERROR;
    if (!IS_GPIO_PIN(cfg->Pin))               return HAL_ERROR;
    if (!IS_GPIO_MODE(cfg->Mode))             return HAL_ERROR;

    GPIO_EnableClock(GPIOx);
    GPIO_PinMode(GPIOx, cfg->Pin, cfg->Mode);

    if (cfg->Mode == OUTPUT) {
        GPIO_Output_Type(GPIOx,  cfg->Pin, cfg->Type);
        GPIO_Output_Speed(GPIOx, cfg->Pin, cfg->Speed);
    } else if (cfg->Mode == INPUT) {
        GPIO_PupPdown(GPIOx, cfg->Pin, cfg->Pull);
    } else if (cfg->Mode == AF) {
        GPIO_Output_Type(GPIOx,  cfg->Pin, cfg->Type);
        GPIO_Output_Speed(GPIOx, cfg->Pin, cfg->Speed);
        GPIO_PupPdown(GPIOx,     cfg->Pin, cfg->Pull);
        GPIO_SetAFmode(GPIOx,    cfg->Pin, cfg->AFx);
    }

    return HAL_OK;
}
void GPIO_EnableClock(GPIO_TypeDef *GPIOx)
{
    assert_param(IS_GPIO_PORT(GPIOx));

    if      (GPIOx == GPIOA) RCC->AHB1ENR |= (1 << 0);
    else if (GPIOx == GPIOB) RCC->AHB1ENR |= (1 << 1);
    else if (GPIOx == GPIOC) RCC->AHB1ENR |= (1 << 2);
    else if (GPIOx == GPIOD) RCC->AHB1ENR |= (1 << 3);
    else if (GPIOx == GPIOE) RCC->AHB1ENR |= (1 << 4);
    else if (GPIOx == GPIOF) RCC->AHB1ENR |= (1 << 5);
    else if (GPIOx == GPIOG) RCC->AHB1ENR |= (1 << 6);
    else if (GPIOx == GPIOH) RCC->AHB1ENR |= (1 << 7);
    else if (GPIOx == GPIOI) RCC->AHB1ENR |= (1 << 8);
}
