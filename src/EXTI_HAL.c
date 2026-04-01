#include "EXTI_HAL.h"

__attribute__((weak)) void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;

    RCC->AHB1ENR |= (1 << 3);
    GPIOD->MODER |= (1 << 28);
    GPIOD->BSRR   = (1 << 14);

    while(1);
}

void EXTI_SetEdge(uint8_t exti_n, Edge_t edge)
{
    assert_param(IS_EXTI_LINE(exti_n));
    assert_param(IS_EXTI_EDGE(edge));

    if (edge == Rising_Edge) {
        EXTI->RTSR |=  (1 << exti_n);
    } else if (edge == Falling_Edge) {
        EXTI->FTSR |=  (1 << exti_n);
    } else {
        EXTI->RTSR |=  (1 << exti_n);
        EXTI->FTSR |=  (1 << exti_n);
    }
}

void EXTI_Unmask(uint8_t exti_n)
{
    assert_param(IS_EXTI_LINE(exti_n));
    EXTI->IMR |= (1 << exti_n);
}

void EXTI_ConfigSYSCFG(uint8_t exti_n, Port_t port)
{
    assert_param(IS_EXTI_LINE(exti_n));
    assert_param(IS_EXTI_PORT(port));

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    uint8_t idx = exti_n / 4;
    uint8_t pos = (exti_n % 4) * 4;

    SYSCFG->EXTICR[idx] = (SYSCFG->EXTICR[idx] & ~(0xF << pos)) | (port << pos);
}

IRQn_Type EXTI_GetIRQn(uint8_t exti_n)
{
    assert_param(IS_EXTI_LINE(exti_n));

    if      (exti_n == 0)                    return EXTI0_IRQn;
    else if (exti_n == 1)                    return EXTI1_IRQn;
    else if (exti_n == 2)                    return EXTI2_IRQn;
    else if (exti_n == 3)                    return EXTI3_IRQn;
    else if (exti_n == 4)                    return EXTI4_IRQn;
    else if (exti_n >= 5 && exti_n <= 9)     return EXTI9_5_IRQn;
    else                                     return EXTI15_10_IRQn;
}

void EXTI_Enable(EXTI_Config_t *cfg)
{
    assert_param(cfg != NULL);
    assert_param(IS_EXTI_LINE(cfg->EXTI_n));
    assert_param(IS_EXTI_EDGE(cfg->Edge));
    assert_param(IS_EXTI_PORT(cfg->Port));

    EXTI_SetEdge(cfg->EXTI_n, cfg->Edge);
    EXTI_ConfigSYSCFG(cfg->EXTI_n, cfg->Port);
    EXTI_Unmask(cfg->EXTI_n);
    NVIC_EnableIRQ(EXTI_GetIRQn(cfg->EXTI_n));
}