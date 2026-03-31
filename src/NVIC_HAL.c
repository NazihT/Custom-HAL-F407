#include "NVIC_HAL.h"

void NVIC_EXTI_EnableEXTI(EXTI_Config_t *EXTI_Pin)
{
    
    NVIC_EXTI_SetEdge(EXTI_Pin->EXTI_n , EXTI_Pin->Edge);
    NVIC_EXTI_SYSCFG(EXTI_Pin->EXTI_n,EXTI_Pin->Port);
    NVIC_EXTI_Unmask(EXTI_Pin->EXTI_n);
    IRQn_Type IRQn= NVIC_EXTI_GetIRQn(EXTI_Pin->EXTI_n);
    NVIC_EnableIRQ(IRQn) ;

}
void NVIC_EXTI_SetEdge(uint8_t EXTI_n , Edge_t Edge)
{
    if (Edge==Rising_Edge)
    {
        EXTI->RTSR |=(1<<EXTI_n);
        

    }
    else if (Edge==Falling_Edge)
    {
        EXTI->FTSR |=(1<<EXTI_n);
        
    }
    else if (Edge==FallingRising_Edge)
    {
        EXTI->RTSR |=(1<<EXTI_n);
        EXTI->FTSR |=(1<<EXTI_n);
        
        
    }

}

void NVIC_EXTI_Unmask(uint8_t EXTI_n)
{
    EXTI->IMR |= (1<<EXTI_n);
}
void NVIC_EXTI_SYSCFG(uint8_t EXTI_n , Port_t Port)
{
    
    RCC->APB2ENR  |= (RCC_APB2ENR_SYSCFGEN);
    uint8_t pos = (EXTI_n%4)*4;
    if (EXTI_n<4)
    {
        SYSCFG->EXTICR [0] = (SYSCFG->EXTICR [0] &~(0xF<<pos)) | (Port<<pos) ;

    }
    else if (EXTI_n > 3 && EXTI_n < 8)
    {
        SYSCFG->EXTICR [1]= (SYSCFG->EXTICR [1] &~(0xF<<pos)) | (Port<<pos) ;

    }
    else if (EXTI_n > 7 && EXTI_n <12)
    {
        SYSCFG->EXTICR [2]= (SYSCFG->EXTICR [2] &~(0xF<<pos)) | (Port<<pos) ;

    }
    else 
    {
        SYSCFG->EXTICR [3]= (SYSCFG->EXTICR [3] &~(0xF<<pos)) | (Port<<pos) ;
    }


    
}

IRQn_Type NVIC_EXTI_GetIRQn(uint8_t EXTIn)
{
    if(EXTIn==0)
    {
        return (EXTI0_IRQn);
    }
    else  if(EXTIn==1)
    {
        return (EXTI1_IRQn);

    }
     else  if(EXTIn==2)
    {
        return (EXTI2_IRQn);
    }

     else  if(EXTIn==3)
    {
        return (EXTI3_IRQn);
    }

     else  if(EXTIn==4)
    {
        return (EXTI4_IRQn);
    }
     else  if(EXTIn > 4 && EXTIn <10)
    {
        return (EXTI9_5_IRQn);
    }
     
    else  
    {
        return (EXTI15_10_IRQn);
    }



}

