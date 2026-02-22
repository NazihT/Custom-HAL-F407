#include "ADC_HAL.h"



void ADC_Init(ADC_TypeDef *adcx , ADC_Config_t *Config)
{
    ADC_ClockEnable(adcx);
    adcx->CR2 |=(1<<ADC_CR2_ADON_Pos);
    ADC_SetResolution(adcx,Config->Resolution);
    
    if (Config->DMA == DMA_Enabled)
    {
         ADC1->CR2 |= ADC_CR2_DMA ;
         ADC1->CR2 |= ADC_CR2_DDS ;
    }

    ADC_SetContinuity(adcx,Config->ModeContinuity);
    ADC_SetSingleScan (adcx,Config->ModeSingleScan);
    if (Config->InterruptMode==Interrupt_Enable) 
    {
        adcx->CR1 |=(1<<ADC_CR1_EOCIE_Pos);
        NVIC_EnableIRQ(ADC_IRQn);
    }
    if (Config->ModeContinuity==Disontinuous_Mode) 
    {
        adcx->CR1 &=~(7 << ADC_CR1_DISCNUM_Pos);
        adcx->CR1 |=(Config->NumChannels << ADC_CR1_DISCNUM_Pos);
    }
    

   ADC1->CR2 |= ADC_CR2_ADON;
}

void ADC_StartConversion(ADC_TypeDef *adcx )
{
    adcx->CR2 |= (1<<ADC_CR2_SWSTART_Pos); 
}
void ADC_Channel_Init(ADC_TypeDef *adcx ,  ADC_Channel_Config_t *Config)
{
  ADC_SetSMPx(adcx,Config->Chx,Config->SMPx);
  ADC_SetSequence(adcx,Config->Chx,Config->SQx);

}



void ADC_SetSequence (ADC_TypeDef *adcx ,  ADC_Channel Chx , uint8_t Sequence)

{
    uint8_t pos = ((Sequence-1)*5)%30;
    if (Sequence < 7 )
    {
        adcx->SQR3 |=(Chx<<pos);
    }
    else  if (Sequence > 6 && Sequence <13 )
    {
        adcx->SQR2 |=(Chx<<pos);
    }
    else  if ( Sequence > 12 )
    {
        adcx->SQR1 |=(Chx<<pos);
    }

}


uint8_t ADC_EOC(ADC_TypeDef *adcx)
{
    if (adcx->SR &  (1<<ADC_SR_EOC_Pos))
    {
        return 1;
    }
    else return 0;
}


void ADC_SetResolution(ADC_TypeDef *ADCx , ADC_Resolution Resolution)
{
    ADCx->CR1 &=~(3<<24);
    ADCx->CR1 |=(Resolution << 24);

    
}
void ADC_SetSMPx(ADC_TypeDef *ADCx , ADC_Channel CHx ,ADC_NbSamples SMP)
{

    uint8_t pos  ;

    if ( CHx < 10)
    {
        pos = CHx*3;
        ADCx->SMPR2 &=~(7 << pos) ;
        ADCx->SMPR2 |= (SMP<<pos);

    }
    else
    {
        pos = (CHx-10)*3;
        ADCx->SMPR1 &=~(7 << pos) ;
        ADCx->SMPR1 |= (SMP<<pos);

    }

}

void ADC_SetContinuity(ADC_TypeDef *ADCx , ADC_Mode_Continuity Mode)
{
    if (Mode==Continuous_Mode)
    {
        ADCx->CR1 &=~(1<<ADC_CR1_DISCEN_Pos) ;
        ADCx->CR2 |=(1<<1);
    }
    else 
    {
        ADCx->CR2  &=~(1<<1);
        ADCx->CR1 |=(1<<ADC_CR1_DISCEN_Pos) ;
    }
}
void ADC_SetSingleScan(ADC_TypeDef*ADCx , ADC_Mode_SingleScan Mode ) 
{
    if (Mode==Single_Mode)
    {
        ADCx->CR1 &=~(1<<ADC_CR1_SCAN_Pos);
    }
    else
    {
        ADCx->CR1 |=(1<<ADC_CR1_SCAN_Pos);
    }
}
void ADC_SetSequenceLength(ADC_TypeDef *adcx, uint8_t length)

{

    adcx->SQR1 &= ~(0xF << 20);  // Clear L[3:0] bits

    adcx->SQR1 |= ((length - 1) << 20);  // Set length (length-1)

}

void ADC_ClockEnable(ADC_TypeDef *adcx)
{
    if (adcx==ADC1)
    {
        RCC->APB2ENR |= RCC_APB2ENR_ADC1EN ;

    }
    else if (adcx==ADC2)
    {
        RCC->APB2ENR |= RCC_APB2ENR_ADC2EN ;
    }
    else if (adcx==ADC3)
    {
        RCC->APB2ENR |= RCC_APB2ENR_ADC3EN ;
    }
}


