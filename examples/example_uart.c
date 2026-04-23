#include "UART_HAL.h"
#include "GPIO_HAL.h"

/*
 * UART Example - STM32F407VG Discovery
 *
 * Example A — TX only, polling:
 *   USART2 TX (PA2), 115200 8N1
 *   Repeatedly transmits "Hello from STM32\r\n" over serial
 *
 * Example B — RX interrupt (echo):
 *   USART2 TX+RX (PA2, PA3), 115200 8N1
 *   Echoes every received byte back, toggles PD12 (green LED) on each byte
 *
 * SYSCLK = 16MHz (HSI)
 * USART2 on APB1 @ 16MHz -> BRR = 16000000 / 115200 = 138
 *
 * Wiring:
 *   PA2 -> UART TX  (AF7)
 *   PA3 -> UART RX  (AF7)
 */

/* ---- Example B globals ---- */
static volatile uint8_t g_rx_byte    = 0;
static volatile uint8_t g_rx_ready   = 0;

void USART2_IRQHandler(void)
{
    // RXNE — data register not empty, byte received
    if (USART2->SR & (1 << USART_SR_RXNE_Pos))
    {
        g_rx_byte  = (uint8_t)USART2->DR;
        g_rx_ready = 1;
    }

    // IDLE — line went idle, useful for framing multi-byte packets
    if (USART2->SR & (1 << USART_SR_IDLE_Pos))
        UART_RxClearFlag(USART2);
}

static void delay(volatile uint32_t n)
{
    while (n--);
}

int main(void)
{
    /* ------------------------------------------------------------------ */
    /* GPIO                                                                 */
    /* ------------------------------------------------------------------ */

    // PA2 — USART2 TX (AF7)
    GPIO_Config_t tx_pin = {
        .Pin   = 2,
        .Mode  = AF,
        .Type  = Push_Pull,
        .Speed = High_Speed,
        .Pull  = No_Pull,
        .AFx   = AF7,
    };
    GPIO_Init(GPIOA, &tx_pin);

    // PA3 — USART2 RX (AF7) — needed for Example B
    GPIO_Config_t rx_pin = {
        .Pin   = 3,
        .Mode  = AF,
        .Type  = Push_Pull,
        .Speed = High_Speed,
        .Pull  = No_Pull,
        .AFx   = AF7,
    };
    GPIO_Init(GPIOA, &rx_pin);

    // PD12 green LED — toggled on each received byte (Example B)
    GPIO_Config_t green_led = {
        .Pin   = 12,
        .Mode  = OUTPUT,
        .Type  = Push_Pull,
        .Speed = Low_Speed,
        .Pull  = No_Pull,
    };
    GPIO_Init(GPIOD, &green_led);

    /* ------------------------------------------------------------------ */
    /* Example A — TX only, polling                                         */
    /* ------------------------------------------------------------------ */

    UART_Config_t uart_tx_cfg = {
        .BaudRate      = 115200,
        .Mode          = UART_MODE_TX,
        .WordLength    = WORD_LENGTH_8BIT,
        .InterruptState = UART_INTERRUPT_DISABLE,
    };
    UART_Init(USART2, &uart_tx_cfg);

    /* ------------------------------------------------------------------ */
    /* Example B — TX+RX, RXNE interrupt echo                              */
    /* ------------------------------------------------------------------ */
    /*
    UART_Config_t uart_rxtx_cfg = {
        .BaudRate       = 115200,
        .Mode           = UART_MODE_TX_RX,
        .WordLength     = WORD_LENGTH_8BIT,
        .InterruptState = UART_INTERRUPT_ENABLE,
    };
    UART_Init(USART2, &uart_rxtx_cfg);
    */

    /* ------------------------------------------------------------------ */
    /* Loop — Example A                                                     */
    /* ------------------------------------------------------------------ */
    while (1)
    {
        UART_TransmitString(USART2, "Hello from STM32\r\n");
        delay(800000);

        /* ---- Example B echo (swap UART init above to use this) ---- */
        /*
        if (g_rx_ready)
        {
            g_rx_ready = 0;
            UART_Transmit(USART2, g_rx_byte);
            GPIO_TogglePin(GPIOD, 12);
        }
        */
    }
}