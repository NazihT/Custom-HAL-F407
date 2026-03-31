#include "GPIO_HAL.h"
#define USE_FULL_ASSERT

/*
 * GPIO Example - STM32F407VG Discovery
 *
 * Demonstrates: output, input with pull-up, alternate function,
 *               read/write/toggle, runtime pin config changes
 *
 * Hardware:
 *   PD12 - green LED (output)
 *   PD13 - orange LED (output)
 *   PA0  - user button (input, pull-down — button pulls high when pressed)
 *   PA2  - USART2 TX (AF7, used to show AF config)
 */

static void delay(volatile uint32_t n)
{
    while (n--);
}

int main(void)
{
    /* --- output: onboard LEDs on PD12, PD13 --- */
    GPIO_Config_t led_green = {
        .Pin   = 12,
        .Mode  = OUTPUT,
        .Type  = Push_Pull,
        .Speed = Low_Speed,
        .Pull  = No_Pull,
    };
    GPIO_Init(GPIOD, &led_green);

    GPIO_Config_t led_orange = {
        .Pin   = 13,
        .Mode  = OUTPUT,
        .Type  = Push_Pull,
        .Speed = Low_Speed,
        .Pull  = No_Pull,
    };
    GPIO_Init(GPIOD, &led_orange);

    /* --- input: user button on PA0 --- */
    GPIO_Config_t button = {
        .Pin  = 0,
        .Mode = INPUT,
        .Pull = Pull_Down,
    };
    GPIO_Init(GPIOA, &button);

    /* --- alternate function: PA2 as USART2_TX (AF7) --- */
    GPIO_Config_t uart_tx = {
        .Pin   = 2,
        .Mode  = AF,
        .Type  = Push_Pull,
        .Speed = High_Speed,
        .Pull  = No_Pull,
        .AFx   = AF7,
    };
    GPIO_Init(GPIOA, &uart_tx);

    /* --- toggle green LED on button press --- */
    /* --- orange LED shows we're alive (blinks freely) --- */
    while (1)
    {
        uint8_t btn_state;
        btn_state=GPIO_ReadPin(GPIOA, 0);

        if (btn_state == 1)
        {
            GPIO_TogglePin(GPIOD, 12);
            delay(200000);   // crude debounce
        }

        GPIO_TogglePin(GPIOD, 13);
        delay(500000);

    }
}