#include "stm32f407xx.h"

typedef enum {
    Falling_Edge,
    Rising_Edge,
    FallingRising_Edge
    
}Edge_t;

typedef enum{
    PortA=0,
    PortB,
    PortC,
    PortD,
    PortE,
    PortF,
    PortG,
    PortH,  

}Port_t ;


typedef struct {
    uint8_t EXTI_n;
    Edge_t Edge;
    Port_t Port;
} EXTI_Config_t;

void NVIC_EXTI_SetEdge(uint8_t EXTI_n , Edge_t Edge);
void NVIC_EXTI_EnableEXTI(EXTI_Config_t *EXTI_Pin);
void NVIC_EXTI_Unmask(uint8_t pin) ;
void NVIC_EXTI_SYSCFG(uint8_t EXTI_n , Port_t Port);
IRQn_Type NVIC_EXTI_GetIRQn(uint8_t EXTIn);

