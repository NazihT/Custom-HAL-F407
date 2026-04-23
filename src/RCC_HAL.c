#include "RCC_HAL.h"

#define HPPRE_Pos  4
#define PPRE1_Pos  10
#define PPRE2_Pos  13


uint32_t SYSCLK_FREQ = 16000000;
uint32_t HCLK_FREQ   = 16000000;
uint32_t PCLK1_FREQ  = 16000000;
uint32_t PCLK2_FREQ  = 16000000;

// index = CFGR encoding value - 7, entry 0 = no division (encoding < 8)
static const uint32_t AHB_div[] = {1, 2, 4, 8, 16, 64, 128, 256, 512};
static const uint32_t APB_div[] = {1, 2, 4, 8, 16};

__attribute__((weak)) void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;

    // red LED PD14 — raw registers, don't call the HAL here
    RCC->AHB1ENR |=  (1 << 3);
    GPIOD->MODER |=  (1 << 28);
    GPIOD->BSRR   =  (1 << 14);

    while(1);
}

void RCC_SetPrescalers(RCC_ConfigPrescaler_t *PSC)
{
    assert_param(PSC != NULL);
    assert_param(IS_AHB_PSC(PSC->AHBPSC));
    assert_param(IS_APB_PSC(PSC->APB1PSC));
    assert_param(IS_APB_PSC(PSC->APB2PSC));

    RCC->CFGR = (RCC->CFGR & ~(0xF << HPPRE_Pos)) | (PSC->AHBPSC << HPPRE_Pos);
    RCC->CFGR = (RCC->CFGR & ~(0x7 << PPRE1_Pos)) | (PSC->APB1PSC << PPRE1_Pos);
    RCC->CFGR = (RCC->CFGR & ~(0x7 << PPRE2_Pos)) | (PSC->APB2PSC << PPRE2_Pos);

    uint8_t ahb_enc  = (RCC->CFGR >> HPPRE_Pos) & 0xF;
    uint8_t apb1_enc = (RCC->CFGR >> PPRE1_Pos) & 0x7;
    uint8_t apb2_enc = (RCC->CFGR >> PPRE2_Pos) & 0x7;

    HCLK_FREQ  = SYSCLK_FREQ / AHB_div[(ahb_enc  < 8) ? 0 : (ahb_enc  - 7)];
    PCLK1_FREQ = HCLK_FREQ   / APB_div[(apb1_enc < 4) ? 0 : (apb1_enc - 3)];
    PCLK2_FREQ = HCLK_FREQ   / APB_div[(apb2_enc < 4) ? 0 : (apb2_enc - 3)];
}

void RCC_UseHSE(void)
{
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    RCC->CFGR = (RCC->CFGR & ~(3 << 0)) | (1 << 0);
    while (!((RCC->CFGR >> 2) & 1));

    RCC->CR &= ~RCC_CR_HSION;
}

void RCC_UseHSI(void)
{
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));

    RCC->CFGR &= ~(3 << 0);
    while ((RCC->CFGR >> 2) & 3);

    RCC->CR &= ~RCC_CR_HSEON;
}

void RCC_ClockInit(RCC_ConfigClock_t *Clock)
{
    assert_param(Clock != NULL);
    assert_param(IS_CLK_SRC(Clock->ClockSource));

    if (Clock->ClockSource == HSE) {
        RCC_UseHSE();
        SYSCLK_FREQ = 8000000;
    } else {
        RCC_UseHSI();
        SYSCLK_FREQ = 16000000;
    }

    RCC_SetPrescalers(&Clock->prescalers);
}