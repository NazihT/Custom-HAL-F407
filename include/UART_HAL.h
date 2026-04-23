#ifndef UART_HAL_H
#define UART_HAL_H

#include "stm32f407xx.h"

/* ---- assert_param ---- */
#ifdef USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t *file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

/* ---- validation macros ---- */
#define IS_UART(uart)         ((uart) == USART1 || (uart) == USART2 || (uart) == USART3 || \
                               (uart) == UART4  || (uart) == UART5  || (uart) == USART6)
#define IS_UART_MODE(m)       ((m) == UART_MODE_TX || (m) == UART_MODE_RX || (m) == UART_MODE_TX_RX)
#define IS_WORD_LENGTH(wl)    ((wl) == WORD_LENGTH_8BIT || (wl) == WORD_LENGTH_9BIT)
#define IS_UART_INTR(s)       ((s) == UART_INTERRUPT_ENABLE || (s) == UART_INTERRUPT_DISABLE)

typedef enum {
    UART_INTERRUPT_DISABLE,
    UART_INTERRUPT_ENABLE,
} UART_Interrupt;

typedef enum {
    WORD_LENGTH_8BIT,
    WORD_LENGTH_9BIT,
} Word_Length;

typedef enum {
    UART_MODE_TX,
    UART_MODE_RX,
    UART_MODE_TX_RX,
} UART_Mode;

typedef struct {
    uint32_t       BaudRate;
    UART_Mode      Mode;
    UART_Interrupt InterruptState;
    Word_Length    WordLength;
} UART_Config_t;

void       UART_Init(USART_TypeDef *uartx, UART_Config_t *cfg);
void       UART_EnableClock(USART_TypeDef *uartx);
void       UART_SetMode(USART_TypeDef *uartx, UART_Mode mode);
void       UART_SetWordLength(USART_TypeDef *uartx, Word_Length wordlength);
void       UART_SetInterrupt(USART_TypeDef *uartx, UART_Interrupt intr);
void       UART_Transmit(USART_TypeDef *uartx, uint8_t data);
void       UART_TransmitString(USART_TypeDef *uartx, const char *string);
void       UART_RxClearFlag(USART_TypeDef *uartx);
IRQn_Type  UART_GetIRQn(USART_TypeDef *uartx);

#endif