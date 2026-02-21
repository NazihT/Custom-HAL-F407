#include "stm32f407xx.h"

extern uint32_t SYSCLK_FREQ ;
extern uint32_t HCLK_FREQ;
extern uint32_t PCLK1_FREQ;
extern uint32_t PCLK2_FREQ;
typedef enum 
{
    APBPSC_2 = 4,
    APBPSC_4= 5,
    APBPSC_8= 6,
    APBPSC_16= 7,
} APBPSC_t ;


typedef enum 
{
    AHBPSC_2 = 8,
    AHBPSC_4= 9,
    AHBPSC_8= 0xA,
    AHBPSC_16= 0xB,
    AHBPSC_64= 0xC,
    AHBPSC_128= 0xD,
    AHBPSC_256= 0xE,
    AHBPSC_512= 0xF,
} AHBPSC_t ;

typedef enum {
    HSE,
    HSI,
    HSExPLL,
    HSIxPLL


}ClockSource_t ;

typedef struct {
    AHBPSC_t AHBPSC;
    APBPSC_t APB1PSC;
    APBPSC_t APB2PSC;

}RCC_ConfigPrescaler_t ;

typedef struct {
    RCC_ConfigPrescaler_t prescalers;
    ClockSource_t ClockSource;


}RCC_ConfigClock_t;

void RCC_SetPrescalers(RCC_ConfigPrescaler_t *PSC);

void RCC_UseHSI();
void RCC_UseHSE();
void RCC_UsePLL(ClockSource_t source);
void RCC_ClockInit(RCC_ConfigClock_t *Clock);
