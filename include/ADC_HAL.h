#ifndef ADC_HAL_H
#define ADC_HAL_H

#include "stm32f407xx.h"

/* ---- assert_param ---- */
#ifdef USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t *file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

/* ---- validation macros ---- */
#define IS_ADC(adc)          ((adc) == ADC1 || (adc) == ADC2 || (adc) == ADC3)
#define IS_ADC_CHANNEL(ch)   ((ch) <= ADC_Channel_15)
#define IS_ADC_RES(r)        ((r) <= _6bit)
#define IS_ADC_SMP(s)        ((s) <= _480_Samples)
#define IS_ADC_SEQ(s)        ((s) >= 1 && (s) <= 16)
#define IS_ADC_SEQLEN(l)     ((l) >= 1 && (l) <= 16)

typedef enum {
    ADC_Channel_0,
    ADC_Channel_1,
    ADC_Channel_2,
    ADC_Channel_3,
    ADC_Channel_4,
    ADC_Channel_5,
    ADC_Channel_6,
    ADC_Channel_7,
    ADC_Channel_8,
    ADC_Channel_9,
    ADC_Channel_10,
    ADC_Channel_11,
    ADC_Channel_12,
    ADC_Channel_13,
    ADC_Channel_14,
    ADC_Channel_15,
} ADC_Channel;

typedef enum {
    _12bit,
    _10bit,
    _8bit,
    _6bit,
} ADC_Resolution;

typedef enum {
    _3_Samples,
    _15_Samples,
    _28_Samples,
    _56_Samples,
    _84_Samples,
    _112_Samples,
    _144_Samples,
    _480_Samples,
} ADC_NbSamples;

typedef enum {
    Continuous_Mode,
    Discontinuous_Mode
} ADC_Mode_Continuity;

typedef enum {
    Single_Mode,
    Scan_Mode
} ADC_Mode_SingleScan;

typedef enum {
    Interrupt_Disable,
    Interrupt_Enable,
} ADC_Mode_Interrupt;

typedef enum {
    DMA_Disabled,
    DMA_Enabled,
} ADC_DMA_Enable_t;
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
typedef struct {
    ADC_Resolution      Resolution;
    ADC_Mode_Continuity ModeContinuity;
    ADC_Mode_SingleScan ModeSingleScan;
    ADC_Mode_Interrupt  InterruptMode;
    ADC_Disc_Num NumChannels;
    ADC_DMA_Enable_t    DMA;
} ADC_Config_t;

typedef struct {
    ADC_Channel   Chx;
    ADC_NbSamples SMPx;
    uint8_t       SQx;
} ADC_Channel_Config_t;

void    ADC_Init(ADC_TypeDef *adcx, ADC_Config_t *cfg);
void    ADC_Channel_Init(ADC_TypeDef *adcx, ADC_Channel_Config_t *cfg);
void    ADC_ClockEnable(ADC_TypeDef *adcx);
void    ADC_StartConversion(ADC_TypeDef *adcx);
uint8_t ADC_EOC(ADC_TypeDef *adcx);
void    ADC_SetResolution(ADC_TypeDef *adcx, ADC_Resolution resolution);
void    ADC_SetSMPx(ADC_TypeDef *adcx, ADC_Channel chx, ADC_NbSamples smp);
void    ADC_SetSequence(ADC_TypeDef *adcx, ADC_Channel chx, uint8_t sequence);
void    ADC_SetSequenceLength(ADC_TypeDef *adcx, uint8_t length);
void    ADC_SetContinuity(ADC_TypeDef *adcx, ADC_Mode_Continuity mode);
void    ADC_SetSingleScan(ADC_TypeDef *adcx, ADC_Mode_SingleScan mode);

#endif