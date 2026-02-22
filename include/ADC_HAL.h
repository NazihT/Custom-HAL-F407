#include "stm32f407xx.h"

typedef enum
{
    ADC_Channel_0 ,
    ADC_Channel_1 ,
    ADC_Channel_2 ,
    ADC_Channel_3 ,
    ADC_Channel_4 ,
    ADC_Channel_5 ,
    ADC_Channel_6 ,
    ADC_Channel_7,
    ADC_Channel_8 ,
    ADC_Channel_9 ,
    ADC_Channel_10 ,
    ADC_Channel_11 ,
    ADC_Channel_12 ,
    ADC_Channel_13 ,
    ADC_Channel_14 ,
    ADC_Channel_15 ,
} ADC_Channel;

typedef enum
{
   _12bit ,
   _10bit ,
   _8bit ,
   _6bit ,


}ADC_Resolution;

typedef enum
{
    _3_Samples,
    _15_Samples,
    _28_Samples,
    _56_Samples,
    _84_Samples,
    _112_Samples,
    _144_Samples,
    _480_Samples,


}ADC_NbSamples;

typedef enum

{
    Continuous_Mode,
    Disontinuous_Mode

} ADC_Mode_Continuity;

typedef enum
{
    Single_Mode,
    Scan_Mode

}
ADC_Mode_SingleScan ;
typedef enum
{
    Interrupt_Disable ,
    Interrupt_Enable ,

}ADC_Mode_Interrupt;


typedef enum 
{
    _1_Channel = 0,
    _2_Channel ,
    _3_Channel ,
    _4_Channel,
    _5_Channel,
    _6_Channel,
    _7_Channel,
    _8_Channel,

}ADC_Disc_Num ;


typedef enum
{
    seq1=0,
    seq2=5,
    seq3=10,
    seq4=15,
    seq5=20,
    seq6=25,


} ADC_Sqx;

typedef enum 
{
    DMA_Disabled,
    DMA_Enabled,

} ADC_DMA_Enable_t;

typedef struct 
{
   ADC_Resolution Resolution;
   ADC_Mode_Continuity ModeContinuity;
   ADC_Mode_SingleScan ModeSingleScan;
   ADC_Mode_Interrupt  InterruptMode;
   ADC_Disc_Num NumChannels;
   ADC_DMA_Enable_t DMA ;
   
} ADC_Config_t;

typedef struct 
{
    ADC_Channel Chx ;
    ADC_NbSamples SMPx;
    uint8_t  SQx ;


}ADC_Channel_Config_t;


void ADC_Channel_Init(ADC_TypeDef *adcx ,  ADC_Channel_Config_t *Config);
void ADC_SetSequence (ADC_TypeDef *adcx ,  ADC_Channel Chx , uint8_t Sequence);


void ADC_ClockEnable(ADC_TypeDef *adcx);
uint8_t ADC_EOC(ADC_TypeDef *adcx);

void ADC_StartConversion(ADC_TypeDef *adcx );
void ADC_Init(ADC_TypeDef  *adcx , ADC_Config_t *Config);

void ADC_CHx_Smp(ADC_TypeDef *adcx , ADC_Channel Chx ,ADC_NbSamples  SamplesNum );

void ADC_SetResolution(ADC_TypeDef *ADCx ,ADC_Resolution);

void ADC_SetSMPx(ADC_TypeDef *ADCx , ADC_Channel CHx ,ADC_NbSamples SMP);

void ADC_SetContinuity(ADC_TypeDef *ADCx , ADC_Mode_Continuity Mode);

void ADC_SetSingleScan(ADC_TypeDef*ADCx , ADC_Mode_SingleScan Mode ) ;
void ADC_SetSequenceLength(ADC_TypeDef *adcx, uint8_t length);