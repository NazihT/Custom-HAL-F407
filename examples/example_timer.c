#include "TIMER_HAL.h"
#include "GPIO_HAL.h"

/*
 * Timer Example - STM32F407VG Discovery
 *
 * TIM4 CH1 -> PD12, PWM, duty cycle sweeps 0-100%
 * TIM2 -> 1Hz interrupt, blinks PD13 (orange LED)
 *
 * SYSCLK = 16MHz (HSI)
 * TIM2:  PSC=15999, ARR=999  -> 1Hz
 * TIM4:  PSC=15999, ARR=999  -> 1kHz PWM
 */

void TIM2_IRQHandler(void)
{
    if (TIM_GetFlagStatus(TIM2))
    {
        GPIO_TogglePin(GPIOD, 13);
        TIM_ClearFlag(TIM2);
    }
}

int main(void)
{
    // PD13 orange LED — blinked by TIM2 interrupt
    GPIO_Config_t led = {
        .Pin   = 13,
        .Mode  = OUTPUT,
        .Type  = Push_Pull,
        .Speed = Low_Speed,
        .Pull  = No_Pull,
    };
    GPIO_Init(GPIOD, &led);

    // PD12 as TIM4_CH1 (AF2) — PWM output
    GPIO_Config_t pwm_pin = {
        .Pin   = 12,
        .Mode  = AF,
        .Type  = Push_Pull,
        .Speed = High_Speed,
        .Pull  = No_Pull,
        .AFx   = AF2,
    };
    GPIO_Init(GPIOD, &pwm_pin);

    // TIM2 — 1Hz interrupt, no PWM
    TIMER_Config_t tim2_cfg = {
        .Mode           = TIMER_MODE_BASIC,
        .PSC            = 15999,
        .ARR            = 999,
        .InterruptState = TIMER_INTERRUPT_ENABLE,
    };
    TIM_Init(TIM2, &tim2_cfg);
    TIM_Start(TIM2);

    // TIM4 CH1 — 1kHz PWM on PD12, start at 0%
    TIMER_Config_t tim4_cfg = {
        .Mode                = TIMER_MODE_PWM,
        .PSC                 = 100,
        .ARR                 = 999,
        .CHx                 = CHANNEL_1,
        .DefaultDuty_Percent = 0,
    };
    TIM_Init(TIM4, &tim4_cfg);
    TIM_Start(TIM4);

    uint8_t duty = 0;
    volatile uint32_t i;

   while (1)
    {
        // breathe up
        for (duty = 0; duty <= 100; duty++)
        {
            TIM_PWM_SetDutyCycle(TIM4, CHANNEL_1, duty);
            for (i = 0; i < 20000; i++);
        }
        // breathe down
        for (duty = 100; duty > 0; duty--)
        {
            TIM_PWM_SetDutyCycle(TIM4, CHANNEL_1, duty);
            for (i = 0; i < 20000; i++);
            
        }
 
        
    }
}