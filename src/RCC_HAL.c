#include "RCC_HAL.h"

#define HPPRE_Pos   4 
#define PPRE1_Pos   10
#define PPRE2_Pos   13


uint32_t SYSCLK_FREQ = 16000000;
uint32_t HCLK_FREQ= 16000000;
uint32_t PCLK1_FREQ= 16000000;
uint32_t PCLK2_FREQ= 16000000;



void RCC_SetPrescalers(RCC_ConfigPrescaler_t *PSC)
{
   RCC->CFGR = (RCC->CFGR & ~(0xF << HPPRE_Pos)) | (PSC->AHBPSC << HPPRE_Pos);
RCC->CFGR = (RCC->CFGR & ~(7 << PPRE1_Pos)) | (PSC->APB1PSC << PPRE1_Pos);
RCC->CFGR = (RCC->CFGR & ~(7 << PPRE2_Pos)) | (PSC->APB2PSC << PPRE2_Pos);
uint8_t ahb_div = (RCC->CFGR >> HPPRE_Pos) & 0xF;
    uint32_t ahb_shift = (ahb_div < 8) ? 0 : (ahb_div - 8 + 1); // matches STM32 ref manual
    HCLK_FREQ = SYSCLK_FREQ >> ahb_shift;

    // PCLK1 = HCLK / APB1 prescaler
    uint8_t apb1_div = (RCC->CFGR >> PPRE1_Pos) & 0x7;
    uint32_t apb1_shift = (apb1_div < 4) ? 0 : (apb1_div - 3);
    PCLK1_FREQ = HCLK_FREQ >> apb1_shift;

    // PCLK2 = HCLK / APB2 prescaler
    uint8_t apb2_div = (RCC->CFGR >> PPRE2_Pos) & 0x7;
    uint32_t apb2_shift = (apb2_div < 4) ? 0 : (apb2_div - 3);
    PCLK2_FREQ = HCLK_FREQ >> apb2_shift;
}

void RCC_UseHSE()
{    
    RCC->CR |=(RCC_CR_HSEON);
    while  (!(RCC->CR & (RCC_CR_HSERDY)));
    RCC->CFGR = (RCC->CFGR &~(3<<0)) | (1<<0);
    while(!((RCC->CFGR & (1<<2))));
    RCC->CR &=~(RCC_CR_HSION);
}

void RCC_UseHSI()
{
     
    RCC->CR |=(RCC_CR_HSION);
    while  (!(RCC->CR & (RCC_CR_HSIRDY)));
    RCC->CFGR = (RCC->CFGR &~(3<<0)) ;
    while(((RCC->CFGR & (3<<2))));
    RCC->CR &=~(RCC_CR_HSEON);
}

void RCC_UsePLL(ClockSource_t source)
{
    if(source == HSE) 
    {
       RCC->CR |=(RCC_CR_HSEON);
       while  (!(RCC->CR & (RCC_CR_HSERDY)));
    } 
    else {
         RCC->CR |=(RCC_CR_HSION);
        while(!(RCC->CR & (RCC_CR_HSIRDY)));
    }
    
    RCC->PLLCFGR = (8 << RCC_PLLCFGR_PLLM_Pos) |   // PLLM = 8
                   (96 << RCC_PLLCFGR_PLLN_Pos) |  // PLLN = 96
                   (1 << RCC_PLLCFGR_PLLP_Pos) |   // PLLP = /4
                   (4 << RCC_PLLCFGR_PLLQ_Pos);    // PLLQ = /4 (for USB, optional)
    
    if( source == HSE) 
    {
        RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
    } 
    else {
        RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC_HSE;
    }
    
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));
    
    FLASH->ACR = FLASH_ACR_LATENCY_2WS;
    
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void RCC_ClockInit(RCC_ConfigClock_t *Clock)
{
    if (Clock->ClockSource==HSE)
    {
        RCC_UseHSE();
        SYSCLK_FREQ =8000000;
    }
    else if (Clock->ClockSource==HSI)
    {
        RCC_UseHSI();
        SYSCLK_FREQ =16000000;
    }
    else if (Clock->ClockSource==HSIxPLL)
    {
        RCC_UsePLL(HSI);
    }
    else if (Clock->ClockSource==HSExPLL)
    {
        RCC_UsePLL(HSE);
    }
    RCC_SetPrescalers(&(Clock->prescalers));
}
