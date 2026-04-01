#ifndef TIMER_HAL_H
#define TIMER_HAL_H

#include "stm32f407xx.h"

/* ---- assert_param ---- */
#ifdef USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t *file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

/* ---- validation macros ---- */
#define IS_TIM(tim)           ((tim) == TIM2 || (tim) == TIM3 || (tim) == TIM4 || \
                               (tim) == TIM5 || (tim) == TIM6 || (tim) == TIM7)
#define IS_PWM_CHANNEL(ch)    ((ch) <= CHANNEL_4)
#define IS_DUTY_PERCENT(pct)  ((pct) <= 100)
#define IS_TIM_MODE(m)        ((m) == TIMER_MODE_BASIC || (m) == TIMER_MODE_PWM)
#define IS_INTR_STATE(s)      ((s) == TIMER_INTERRUPT_ENABLE || (s) == TIMER_INTERRUPT_DISABLE)

typedef enum {
    TIMER_INTERRUPT_ENABLE,
    TIMER_INTERRUPT_DISABLE
} Interrupt_State;

typedef enum {
    CHANNEL_1,
    CHANNEL_2,
    CHANNEL_3,
    CHANNEL_4,
} PWM_Channel;

typedef enum {
    TIMER_MODE_BASIC,
    TIMER_MODE_PWM
} Timer_MODE;

typedef struct {
    Timer_MODE      Mode;
    Interrupt_State InterruptState;
    uint16_t        PSC;
    uint16_t        ARR;
    uint8_t         DefaultDuty_Percent;
    PWM_Channel     CHx;
} TIMER_Config_t;

void       TIM_Init(TIM_TypeDef *TIMx, TIMER_Config_t *cfg);
void       TIM_EnableClock(TIM_TypeDef *TIMx);
void       TIM_Start(TIM_TypeDef *TIMx);
void       TIM_Stop(TIM_TypeDef *TIMx);
void       TIM_EnableInterrupt(TIM_TypeDef *TIMx, Interrupt_State intr);
void       TIM_PWM_EnableChannel(TIM_TypeDef *TIMx, PWM_Channel channel);
void       TIM_PWM_SetDutyCycle(TIM_TypeDef *TIMx, PWM_Channel channel, uint8_t duty_percent);
uint8_t    TIM_GetFlagStatus(TIM_TypeDef *TIMx);
void       TIM_ClearFlag(TIM_TypeDef *TIMx);
void       TIM_ResetCounter(TIM_TypeDef *TIMx);
uint32_t   TIM_GetCounter(TIM_TypeDef *TIMx);
void       TIM_SetCounter(TIM_TypeDef *TIMx, uint32_t value);
void       TIM_SetPrescaler(TIM_TypeDef *TIMx, uint16_t prescaler);
void       TIM_SetAutoReload(TIM_TypeDef *TIMx, uint32_t auto_reload);
void       TIM_OnePulseMode(TIM_TypeDef *TIMx);
IRQn_Type  TIM_GetIRQn(TIM_TypeDef *TIMx);

#endif