#include "TIMER_HAL.h"

void TIM_Init(TIM_TypeDef *TIMx , TIMER_Config_t *ConfigStruct )
{
    if(ConfigStruct->Mode==TIMER_MODE_BASIC)
    {
      TIM_EnableClock(TIMx);
      TIMx->PSC = ConfigStruct->PSC;
      TIMx->ARR = ConfigStruct->ARR;
      TIM_EnableInterrupt(TIMx,ConfigStruct->InterruptState);
    }
    else if (ConfigStruct->Mode==TIMER_MODE_PWM)
    {
          TIM_EnableClock(TIMx);
          TIMx->PSC= ConfigStruct ->PSC;
          TIMx->ARR= ConfigStruct ->ARR;
          TIM_PWM_SetDutyCycle(TIMx,ConfigStruct->CHx,ConfigStruct->DefaultDuty);
          TIM_PWM_EnableChannel(TIMx,ConfigStruct->CHx);
    }
}


void TIM_PWM_EnableChannel(TIM_TypeDef *TIMx, PWM_Channel Channel)
{
    if (Channel == CHANNEL_1)
    {
        TIMx->CCMR1 &=~(7<<4); 
        TIMx->CCMR1 |=(6<<4);
        TIMx->CCMR1 |=(1<<3);
        TIMx->CCER |=(1<<0);


    }
    else if (Channel==CHANNEL_2)
    {
        TIMx->CCMR1 &=~(7<<12); 
        TIMx->CCMR1 |=(6<<12);
        TIMx->CCMR1 |=(1<<11);
        TIMx->CCER |=(1<<4);
    }
     else if (Channel==CHANNEL_3)
    {
        TIMx->CCMR2 &=~(7<<4); 
        TIMx->CCMR2 |=(6<<4);
        TIMx->CCMR2 |=(1<<3);
        TIMx->CCER |=(1<<8);

    }
     else if (Channel==CHANNEL_4)
    {
        TIMx->CCMR2 &=~(7<<12); 
        TIMx->CCMR2 |=(6<<12);
        TIMx->CCMR2 |=(1<<11);
        TIMx->CCER |=(1<<12);

    }
    TIMx->CR1 |= (1<<7);


}


void TIM_PWM_SetDutyCycle(TIM_TypeDef *TIMx , PWM_Channel Channel, uint16_t DutyCycle)
{
    if (Channel==CHANNEL_1)
    {
      TIMx->CCR1 = (TIMx->ARR * DutyCycle)/100;
    }
    else if (Channel==CHANNEL_2)
    {
       TIMx->CCR2 = (TIMx->ARR * DutyCycle)/100;
  
    }
    else if (Channel==CHANNEL_3)
    {
       TIMx->CCR3 = (TIMx->ARR * DutyCycle)/100;
    }
    else if (Channel==CHANNEL_4)
    {
        TIMx->CCR4 = (TIMx->ARR * DutyCycle)/100;
 
    }

}
void TIM_EnableInterrupt(TIM_TypeDef *TIMx , Interrupt_State Intr  )
{
   uint8_t IRQnumber=TIM_GetIRQn(TIMx);

   if (Intr==TIMER_INTERRUPT_ENABLE)
    {
        TIMx->DIER |=(1<<0);
        TIMx->SR &=~(1<<0);
        NVIC_EnableIRQ(IRQnumber);
        
    }
   else 
    {
        TIMx->DIER &=~ (1<<0);
        NVIC_DisableIRQ(IRQnumber);
    }
}



void TIM_Start(TIM_TypeDef *TIMx)
{
    TIMx->CNT=0;
    TIMx->CR1 |=(1<<0);
}
void TIM_Stop(TIM_TypeDef *TIMx)
{
    TIMx->CNT=0;
    TIMx->CR1 &=~(1<<0);
}

uint8_t TIM_GetFlagStatus(TIM_TypeDef *TIMx) {
    return (TIMx->SR & (1 << 0)) ? 1 : 0;
}
void TIM_ClearFlag(TIM_TypeDef *TIMx) 
{
    TIMx->SR &=~(1<<0);
}
void TIM_ResetCounter(TIM_TypeDef *TIMx)
{
    TIMx->CNT=0;
}

uint32_t TIM_GetCounter(TIM_TypeDef *TIMx)
{
    return(TIMx->CNT);
}
void TIM_SetCounter(TIM_TypeDef *TIMx, uint32_t value)
{
    TIMx->CNT=value;
}
void TIM_SetPrescaler(TIM_TypeDef *TIMx, uint16_t prescaler)
{
    TIMx->PSC = prescaler;
}
void TIM_SetAutoReload(TIM_TypeDef *TIMx, uint32_t auto_reload)
{
    TIMx->ARR=auto_reload;
}
void TIM_OnePulseMode(TIM_TypeDef *TIMx)
{
    TIMx->CR1 |=(1<<3);
}


void TIM_EnableClock(TIM_TypeDef *TIMx)
{
   if (TIMx == TIM2) RCC->APB1ENR |= (1 << 0);
    else if (TIMx == TIM3) RCC->APB1ENR |= (1 << 1);
    else if (TIMx == TIM4) RCC->APB1ENR |= (1 << 2);
    else if (TIMx == TIM5) RCC->APB1ENR |= (1 << 3);
    else if (TIMx == TIM6) RCC->APB1ENR |= (1 << 4);
    else if (TIMx == TIM7) RCC->APB1ENR |= (1 << 5);
    

}
IRQn_Type TIM_GetIRQn(TIM_TypeDef *TIMx)
{
    if (TIMx == TIM2) return TIM2_IRQn;
    else if (TIMx == TIM3) return TIM3_IRQn;
    else if (TIMx == TIM4) return TIM4_IRQn;
    else if (TIMx == TIM5) return TIM5_IRQn;
    else if (TIMx == TIM6) return TIM6_DAC_IRQn;
    else if (TIMx == TIM7) return TIM7_IRQn;
  
    else  while(1);
   

}























