#include "UART_HAL.h"

#define PCLK1_FREQ  16000000U
#define PCLK2_FREQ  16000000U

void UART_Init(USART_TypeDef *uartx, UART_Config_t *cfg)
{
    assert_param(IS_UART(uartx));
    assert_param(cfg != NULL);
    assert_param(IS_UART_MODE(cfg->Mode));
    assert_param(IS_WORD_LENGTH(cfg->WordLength));
    assert_param(IS_UART_INTR(cfg->InterruptState));

    UART_EnableClock(uartx);

    if (uartx == USART1 || uartx == USART6)
        uartx->BRR = PCLK2_FREQ / cfg->BaudRate;
    else
        uartx->BRR = PCLK1_FREQ / cfg->BaudRate;

    UART_SetWordLength(uartx, cfg->WordLength);
    UART_SetMode(uartx, cfg->Mode);
    UART_SetInterrupt(uartx, cfg->InterruptState);

    uartx->CR1 |= (1 << USART_CR1_UE_Pos);
}

void UART_SetMode(USART_TypeDef *uartx, UART_Mode mode)
{
    assert_param(IS_UART(uartx));
    assert_param(IS_UART_MODE(mode));

    if (mode == UART_MODE_TX) {
        uartx->CR1 |=  (1 << USART_CR1_TE_Pos);
        uartx->CR1 &= ~(1 << USART_CR1_RE_Pos);
    } else if (mode == UART_MODE_RX) {
        uartx->CR1 |=  (1 << USART_CR1_RE_Pos);
        uartx->CR1 &= ~(1 << USART_CR1_TE_Pos);
    } else {
        uartx->CR1 |= (1 << USART_CR1_TE_Pos) | (1 << USART_CR1_RE_Pos);
    }
}

void UART_SetWordLength(USART_TypeDef *uartx, Word_Length wordlength)
{
    assert_param(IS_UART(uartx));
    assert_param(IS_WORD_LENGTH(wordlength));

    if (wordlength == WORD_LENGTH_8BIT)
        uartx->CR1 &= ~(1 << USART_CR1_M_Pos);
    else
        uartx->CR1 |=  (1 << USART_CR1_M_Pos);
}

void UART_SetInterrupt(USART_TypeDef *uartx, UART_Interrupt intr)
{
    assert_param(IS_UART(uartx));
    assert_param(IS_UART_INTR(intr));

    if (intr == UART_INTERRUPT_ENABLE) {
        uartx->CR1 |= (1 << USART_CR1_RXNEIE_Pos) | (1 << USART_CR1_IDLEIE_Pos);
        NVIC_EnableIRQ(UART_GetIRQn(uartx));
    } else {
        uartx->CR1 &= ~((1 << USART_CR1_RXNEIE_Pos) | (1 << USART_CR1_IDLEIE_Pos));
        NVIC_DisableIRQ(UART_GetIRQn(uartx));
    }
}

void UART_Transmit(USART_TypeDef *uartx, uint8_t data)
{
    assert_param(IS_UART(uartx));

    while (!(uartx->SR & (1 << USART_SR_TXE_Pos)));
    uartx->DR = data;
}

void UART_TransmitString(USART_TypeDef *uartx, const char *string)
{
    assert_param(IS_UART(uartx));
    assert_param(string != NULL);

    while (*string)
        UART_Transmit(uartx, (uint8_t)*string++);
}

void UART_RxClearFlag(USART_TypeDef *uartx)
{
    assert_param(IS_UART(uartx));
    (void)uartx->DR;
}

IRQn_Type UART_GetIRQn(USART_TypeDef *uartx)
{
    assert_param(IS_UART(uartx));

    if      (uartx == USART1) return USART1_IRQn;
    else if (uartx == USART2) return USART2_IRQn;
    else if (uartx == USART3) return USART3_IRQn;
    else if (uartx == UART4)  return UART4_IRQn;
    else if (uartx == UART5)  return UART5_IRQn;
    else                      return USART6_IRQn;
}

void UART_EnableClock(USART_TypeDef *uartx)
{
    assert_param(IS_UART(uartx));

    if      (uartx == USART1) RCC->APB2ENR |= (1 << RCC_APB2ENR_USART1EN_Pos);
    else if (uartx == USART2) RCC->APB1ENR |= (1 << RCC_APB1ENR_USART2EN_Pos);
    else if (uartx == USART3) RCC->APB1ENR |= (1 << RCC_APB1ENR_USART3EN_Pos);
    else if (uartx == UART4)  RCC->APB1ENR |= (1 << RCC_APB1ENR_UART4EN_Pos);
    else if (uartx == UART5)  RCC->APB1ENR |= (1 << RCC_APB1ENR_UART5EN_Pos);
    else                      RCC->APB2ENR |= (1 << RCC_APB2ENR_USART6EN_Pos);
}