#include "TIMER_HAL.h"

__attribute__((weak)) void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;

    RCC->AHB1ENR |= (1 << 3);
    GPIOD->MODER |= (1 << 28);
    GPIOD->BSRR   = (1 << 14);

    while(1);
}


void TIM_EnableInterrupt(TIM_TypeDef *TIMx, Interrupt_State intr)
{
    assert_param(IS_TIM(TIMx));
    assert_param(IS_INTR_STATE(intr));

    if (intr == TIMER_INTERRUPT_ENABLE) {
        TIMx->DIER |=  (1 << 0);
        TIMx->SR   &= ~(1 << 0);
        NVIC_EnableIRQ(TIM_GetIRQn(TIMx));
    } else {
        TIMx->DIER &= ~(1 << 0);
        NVIC_DisableIRQ(TIM_GetIRQn(TIMx));
    }
}

void TIM_PWM_EnableChannel(TIM_TypeDef *TIMx, PWM_Channel channel)
{
    assert_param(IS_TIM(TIMx));
    assert_param(IS_PWM_CHANNEL(channel));

    if (channel == CHANNEL_1) {
        TIMx->CCMR1 &= ~(7 << 4);
        TIMx->CCMR1 |=  (6 << 4) | (1 << 3);
        TIMx->CCER  |=  (1 << 0);
    } else if (channel == CHANNEL_2) {
        TIMx->CCMR1 &= ~(7 << 12);
        TIMx->CCMR1 |=  (6 << 12) | (1 << 11);
        TIMx->CCER  |=  (1 << 4);
    } else if (channel == CHANNEL_3) {
        TIMx->CCMR2 &= ~(7 << 4);
        TIMx->CCMR2 |=  (6 << 4) | (1 << 3);
        TIMx->CCER  |=  (1 << 8);
    } else {
        TIMx->CCMR2 &= ~(7 << 12);
        TIMx->CCMR2 |=  (6 << 12) | (1 << 11);
        TIMx->CCER  |=  (1 << 12);
    }

    TIMx->CR1 |= (1 << 7);
}

void TIM_PWM_SetDutyCycle(TIM_TypeDef *TIMx, PWM_Channel channel, uint8_t duty_percent)
{
    assert_param(IS_TIM(TIMx));
    assert_param(IS_PWM_CHANNEL(channel));
    assert_param(IS_DUTY_PERCENT(duty_percent));

    if      (channel == CHANNEL_1) TIMx->CCR1 = (TIMx->ARR * duty_percent) / 100;
    else if (channel == CHANNEL_2) TIMx->CCR2 = (TIMx->ARR * duty_percent) / 100;
    else if (channel == CHANNEL_3) TIMx->CCR3 = (TIMx->ARR * duty_percent) / 100;
    else                           TIMx->CCR4 = (TIMx->ARR * duty_percent) / 100;
}

void TIM_Init(TIM_TypeDef *TIMx, TIMER_Config_t *cfg)
{
    assert_param(IS_TIM(TIMx));
    assert_param(cfg != NULL);
    assert_param(IS_TIM_MODE(cfg->Mode));

    TIM_EnableClock(TIMx);
    TIMx->PSC = cfg->PSC;
    TIMx->ARR = cfg->ARR;

    if (cfg->Mode == TIMER_MODE_BASIC) {
        TIM_EnableInterrupt(TIMx, cfg->InterruptState);
    } else {
        assert_param(IS_PWM_CHANNEL(cfg->CHx));
        assert_param(IS_DUTY_PERCENT(cfg->DefaultDuty_Percent));
        TIM_PWM_SetDutyCycle(TIMx, cfg->CHx, cfg->DefaultDuty_Percent);
        TIM_PWM_EnableChannel(TIMx, cfg->CHx);
    }
}

void TIM_Start(TIM_TypeDef *TIMx)
{
    assert_param(IS_TIM(TIMx));
    TIMx->CNT = 0;
    TIMx->CR1 |= (1 << 0);
}

void TIM_Stop(TIM_TypeDef *TIMx)
{
    assert_param(IS_TIM(TIMx));
    TIMx->CNT = 0;
    TIMx->CR1 &= ~(1 << 0);
}

uint8_t TIM_GetFlagStatus(TIM_TypeDef *TIMx)
{
    assert_param(IS_TIM(TIMx));
    return (TIMx->SR & (1 << 0)) ? 1 : 0;
}

void TIM_ClearFlag(TIM_TypeDef *TIMx)
{
    assert_param(IS_TIM(TIMx));
    TIMx->SR &= ~(1 << 0);
}

void TIM_ResetCounter(TIM_TypeDef *TIMx)
{
    assert_param(IS_TIM(TIMx));
    TIMx->CNT = 0;
}

uint32_t TIM_GetCounter(TIM_TypeDef *TIMx)
{
    assert_param(IS_TIM(TIMx));
    return TIMx->CNT;
}

void TIM_SetCounter(TIM_TypeDef *TIMx, uint32_t value)
{
    assert_param(IS_TIM(TIMx));
    TIMx->CNT = value;
}

void TIM_SetPrescaler(TIM_TypeDef *TIMx, uint16_t prescaler)
{
    assert_param(IS_TIM(TIMx));
    TIMx->PSC = prescaler;
}

void TIM_SetAutoReload(TIM_TypeDef *TIMx, uint32_t auto_reload)
{
    assert_param(IS_TIM(TIMx));
    TIMx->ARR = auto_reload;
}

void TIM_OnePulseMode(TIM_TypeDef *TIMx)
{
    assert_param(IS_TIM(TIMx));
    TIMx->CR1 |= (1 << 3);
}
void TIM_EnableClock(TIM_TypeDef *TIMx)
{
    assert_param(IS_TIM(TIMx));

    if      (TIMx == TIM2) RCC->APB1ENR |= (1 << 0);
    else if (TIMx == TIM3) RCC->APB1ENR |= (1 << 1);
    else if (TIMx == TIM4) RCC->APB1ENR |= (1 << 2);
    else if (TIMx == TIM5) RCC->APB1ENR |= (1 << 3);
    else if (TIMx == TIM6) RCC->APB1ENR |= (1 << 4);
    else if (TIMx == TIM7) RCC->APB1ENR |= (1 << 5);
}

IRQn_Type TIM_GetIRQn(TIM_TypeDef *TIMx)
{
    assert_param(IS_TIM(TIMx));

    if      (TIMx == TIM2) return TIM2_IRQn;
    else if (TIMx == TIM3) return TIM3_IRQn;
    else if (TIMx == TIM4) return TIM4_IRQn;
    else if (TIMx == TIM5) return TIM5_IRQn;
    else if (TIMx == TIM6) return TIM6_DAC_IRQn;
    else                   return TIM7_IRQn;
}
