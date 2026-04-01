#ifndef RCC_HAL_H
#define RCC_HAL_H

#include "stm32f407xx.h"

/* ---- assert_param ---- */
#ifdef USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t *file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

/* ---- validation macros ---- */
#define IS_AHB_PSC(x)   ((x) == AHBPSC_2  || (x) == AHBPSC_4  || (x) == AHBPSC_8  || \
                         (x) == AHBPSC_16  || (x) == AHBPSC_64 || (x) == AHBPSC_128 || \
                         (x) == AHBPSC_256 || (x) == AHBPSC_512)
#define IS_APB_PSC(x)   ((x) == APBPSC_2  || (x) == APBPSC_4  || \
                         (x) == APBPSC_8  || (x) == APBPSC_16)
#define IS_CLK_SRC(x)   ((x) == HSE || (x) == HSI)

/* ---- clock frequency globals (used by all peripherals) ---- */
extern uint32_t SYSCLK_FREQ;
extern uint32_t HCLK_FREQ;
extern uint32_t PCLK1_FREQ;
extern uint32_t PCLK2_FREQ;

typedef enum {
    APBPSC_2  = 4,
    APBPSC_4  = 5,
    APBPSC_8  = 6,
    APBPSC_16 = 7,
} APBPSC_t;

typedef enum {
    AHBPSC_2   = 8,
    AHBPSC_4   = 9,
    AHBPSC_8   = 0xA,
    AHBPSC_16  = 0xB,
    AHBPSC_64  = 0xC,
    AHBPSC_128 = 0xD,
    AHBPSC_256 = 0xE,
    AHBPSC_512 = 0xF,
} AHBPSC_t;

typedef enum {
    HSE,
    HSI,
} ClockSource_t;

typedef struct {
    AHBPSC_t AHBPSC;
    APBPSC_t APB1PSC;
    APBPSC_t APB2PSC;
} RCC_ConfigPrescaler_t;

typedef struct {
    RCC_ConfigPrescaler_t prescalers;
    ClockSource_t         ClockSource;
} RCC_ConfigClock_t;

void RCC_SetPrescalers(RCC_ConfigPrescaler_t *PSC);
void RCC_UseHSI(void);
void RCC_UseHSE(void);
void RCC_ClockInit(RCC_ConfigClock_t *Clock);

#endif