#include "stm32f407xx.h"

typedef enum{
    TIMER_INTERRUPT_ENABLE,
    TIMER_INTERRUPT_DISABLE

}Interrupt_State;

typedef enum {
    CHANNEL_1,
    CHANNEL_2,
    CHANNEL_3,
    CHANNEL_4,

}PWM_Channel;
typedef enum {
    TIMER_MODE_BASIC,
    TIMER_MODE_PWM
} Timer_MODE;


typedef struct {

    Timer_MODE  Mode;
    Interrupt_State InterruptState;
    uint16_t PSC;
    uint16_t ARR;
    uint8_t DefaultDuty;
    PWM_Channel CHx;

} TIMER_Config_t;


void TIM_Init(TIM_TypeDef *TIMx , TIMER_Config_t *ConfigStruct );





void TIM_PWM_EnableChannel(TIM_TypeDef *TIMx, PWM_Channel Channel);
void TIM_PWM_SetDutyCycle(TIM_TypeDef *TIMx , PWM_Channel Channel, uint16_t DutyCycle);

void TIM_EnableClock(TIM_TypeDef *TIMx);
void TIM_Start(TIM_TypeDef *TIMx);
void TIM_Stop(TIM_TypeDef *TIMx);
IRQn_Type TIM_GetIRQn(TIM_TypeDef *TIMx);
void TIM_EnableInterrupt(TIM_TypeDef *TIMx , Interrupt_State Intr  );
uint8_t TIM_GetFlagStatus(TIM_TypeDef *TIMx) ;
void TIM_ClearFlag(TIM_TypeDef *TIMx) ;
void TIM_ResetCounter(TIM_TypeDef *TIMx);
uint32_t TIM_GetCounter(TIM_TypeDef *TIMx);
void TIM_SetCounter(TIM_TypeDef *TIMx, uint32_t value);
void TIM_SetPrescaler(TIM_TypeDef *TIMx, uint16_t prescaler);
void TIM_SetAutoReload(TIM_TypeDef *TIMx, uint32_t auto_reload);
void TIM_OnePulseMode(TIM_TypeDef *TIMx);

