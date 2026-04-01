#include "ADC_HAL.h"

__attribute__((weak)) void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;

    RCC->AHB1ENR |= (1 << 3);
    GPIOD->MODER |= (1 << 28);
    GPIOD->BSRR   = (1 << 14);

    while(1);
}

void ADC_ClockEnable(ADC_TypeDef *adcx)
{
    assert_param(IS_ADC(adcx));

    if      (adcx == ADC1) RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    else if (adcx == ADC2) RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;
    else if (adcx == ADC3) RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
}

void ADC_SetResolution(ADC_TypeDef *adcx, ADC_Resolution resolution)
{
    assert_param(IS_ADC(adcx));
    assert_param(IS_ADC_RES(resolution));

    adcx->CR1 &= ~(3 << 24);
    adcx->CR1 |=  (resolution << 24);
}

void ADC_SetSMPx(ADC_TypeDef *adcx, ADC_Channel chx, ADC_NbSamples smp)
{
    assert_param(IS_ADC(adcx));
    assert_param(IS_ADC_CHANNEL(chx));
    assert_param(IS_ADC_SMP(smp));

    uint8_t pos;

    if (chx < 10) {
        pos = chx * 3;
        adcx->SMPR2 &= ~(7 << pos);
        adcx->SMPR2 |=  (smp << pos);
    } else {
        pos = (chx - 10) * 3;
        adcx->SMPR1 &= ~(7 << pos);
        adcx->SMPR1 |=  (smp << pos);
    }
}

void ADC_SetSequence(ADC_TypeDef *adcx, ADC_Channel chx, uint8_t sequence)
{
    assert_param(IS_ADC(adcx));
    assert_param(IS_ADC_CHANNEL(chx));
    assert_param(IS_ADC_SEQ(sequence));

    uint8_t pos = ((sequence - 1) * 5) % 30;

    if      (sequence <= 6)  adcx->SQR3 |= (chx << pos);
    else if (sequence <= 12) adcx->SQR2 |= (chx << pos);
    else                     adcx->SQR1 |= (chx << pos);
}

void ADC_SetSequenceLength(ADC_TypeDef *adcx, uint8_t length)
{
    assert_param(IS_ADC(adcx));
    assert_param(IS_ADC_SEQLEN(length));

    adcx->SQR1 &= ~(0xF << 20);
    adcx->SQR1 |=  ((length - 1) << 20);
}

void ADC_SetContinuity(ADC_TypeDef *adcx, ADC_Mode_Continuity mode)
{
    assert_param(IS_ADC(adcx));

    if (mode == Continuous_Mode) {
        adcx->CR1 &= ~(1 << ADC_CR1_DISCEN_Pos);
        adcx->CR2 |=  (1 << 1);
    } else {
        adcx->CR2 &= ~(1 << 1);
        adcx->CR1 |=  (1 << ADC_CR1_DISCEN_Pos);
    }
}

void ADC_SetSingleScan(ADC_TypeDef *adcx, ADC_Mode_SingleScan mode)
{
    assert_param(IS_ADC(adcx));

    if (mode == Single_Mode)
        adcx->CR1 &= ~(1 << ADC_CR1_SCAN_Pos);
    else
        adcx->CR1 |=  (1 << ADC_CR1_SCAN_Pos);
}

void ADC_Channel_Init(ADC_TypeDef *adcx, ADC_Channel_Config_t *cfg)
{
    assert_param(IS_ADC(adcx));
    assert_param(cfg != NULL);
    assert_param(IS_ADC_CHANNEL(cfg->Chx));
    assert_param(IS_ADC_SMP(cfg->SMPx));
    assert_param(IS_ADC_SEQ(cfg->SQx));

    ADC_SetSMPx(adcx, cfg->Chx, cfg->SMPx);
    ADC_SetSequence(adcx, cfg->Chx, cfg->SQx);
}

void ADC_Init(ADC_TypeDef *adcx, ADC_Config_t *cfg)
{
    assert_param(IS_ADC(adcx));
    assert_param(cfg != NULL);

    ADC_ClockEnable(adcx);

    adcx->CR2 |= ADC_CR2_ADON;

    ADC_SetResolution(adcx, cfg->Resolution);
    ADC_SetContinuity(adcx, cfg->ModeContinuity);
    ADC_SetSingleScan(adcx, cfg->ModeSingleScan);

    if (cfg->DMA == DMA_Enabled) {
        adcx->CR2 |= ADC_CR2_DMA;
        adcx->CR2 |= ADC_CR2_DDS;
    }

    if (cfg->InterruptMode == Interrupt_Enable) {
        adcx->CR1 |= (1 << ADC_CR1_EOCIE_Pos);
        NVIC_EnableIRQ(ADC_IRQn);
    }

    if (cfg->ModeContinuity == Discontinuous_Mode) {
        adcx->CR1 &= ~(7 << ADC_CR1_DISCNUM_Pos);
        adcx->CR1 |=  (cfg->NumChannels << ADC_CR1_DISCNUM_Pos);
    }
}

void ADC_StartConversion(ADC_TypeDef *adcx)
{
    assert_param(IS_ADC(adcx));
    adcx->CR2 |= (1 << ADC_CR2_SWSTART_Pos);
}

uint8_t ADC_EOC(ADC_TypeDef *adcx)
{
    assert_param(IS_ADC(adcx));
    return (adcx->SR & (1 << ADC_SR_EOC_Pos)) ? 1 : 0;
}