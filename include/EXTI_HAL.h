#ifndef EXTI_HAL_H
#define EXTI_HAL_H

#include "stm32f407xx.h"

/* ---- assert_param ---- */
#ifdef USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t *file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

/* ---- validation macros ---- */
#define IS_EXTI_LINE(n)   ((n) <= 15)
#define IS_EXTI_EDGE(e)   ((e) == Falling_Edge || (e) == Rising_Edge || (e) == FallingRising_Edge)
#define IS_EXTI_PORT(p)   ((p) <= PortH)

typedef enum {
    Falling_Edge,
    Rising_Edge,
    FallingRising_Edge
} Edge_t;

typedef enum {
    PortA = 0,
    PortB,
    PortC,
    PortD,
    PortE,
    PortF,
    PortG,
    PortH,
} Port_t;

typedef struct {
    uint8_t EXTI_n;
    Edge_t  Edge;
    Port_t  Port;
} EXTI_Config_t;

void      EXTI_Enable(EXTI_Config_t *cfg);
void      EXTI_SetEdge(uint8_t exti_n, Edge_t edge);
void      EXTI_Unmask(uint8_t exti_n);
void      EXTI_ConfigSYSCFG(uint8_t exti_n, Port_t port);
IRQn_Type EXTI_GetIRQn(uint8_t exti_n);

#endif