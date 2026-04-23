#include "ADC_HAL.h"
#include "GPIO_HAL.h"

/*
 * ADC Example - STM32F407VG Discovery
 *
 * Example A — Polling:
 *   ADC1 CH1 (PA1), single conversion, 12-bit
 *   Reads a potentiometer, maps result to blink delay on PD12 (green LED)
 *
 * Example B — Interrupt:
 *   ADC1 CH1 (PA1), continuous scan, EOC interrupt
 *   Stores latest sample in g_adc_result, lights PD14 (red LED) past threshold
 *
 * SYSCLK = 16MHz (HSI)
 * ADC result range: 0–4095 (12-bit)
 */

/* ---- Example B globals ---- */
static volatile uint16_t g_adc_result = 0;

void ADC_IRQHandler(void)
{
    g_adc_result = ADC1->DR;
}

/* ---- helpers ---- */
static void delay(volatile uint32_t n)
{
    while (n--);
}

int main(void)
{
    /* ------------------------------------------------------------------ */
    /* GPIO                                                                 */
    /* ------------------------------------------------------------------ */

    // PA1 — analog input (potentiometer wiper)
    GPIO_Config_t pot_pin = {
        .Pin  = 1,
        .Mode = ANALOG,
    };
    GPIO_Init(GPIOA, &pot_pin);

    // PD12 green LED — blink speed driven by pot (Example A)
    GPIO_Config_t green_led = {
        .Pin   = 12,
        .Mode  = OUTPUT,
        .Type  = Push_Pull,
        .Speed = Low_Speed,
        .Pull  = No_Pull,
    };
    GPIO_Init(GPIOD, &green_led);

    // PD14 red LED — threshold indicator (Example B)
    GPIO_Config_t red_led = {
        .Pin   = 14,
        .Mode  = OUTPUT,
        .Type  = Push_Pull,
        .Speed = Low_Speed,
        .Pull  = No_Pull,
    };
    GPIO_Init(GPIOD, &red_led);

    /* ------------------------------------------------------------------ */
    /* Example A — single conversion, polling                              */
    /* ------------------------------------------------------------------ */

    ADC_Config_t adc_poll_cfg = {
        .Resolution     = _12bit,
        .ModeContinuity = Discontinuous_Mode,
        .ModeSingleScan = Single_Mode,
        .DMA            = DMA_Disabled,
        .InterruptMode  = Interrupt_Disable,
        .NumChannels    = 1,
    };
    ADC_Init(ADC1, &adc_poll_cfg);

    ADC_Channel_Config_t ch1_cfg = {
        .Chx  = ADC_Channel_1,
        .SMPx = _84_Samples,
        .SQx  = 1,
    };
    ADC_Channel_Init(ADC1, &ch1_cfg);
    ADC_SetSequenceLength(ADC1, 1);

    /* ------------------------------------------------------------------ */
    /* Example B — continuous, EOC interrupt                               */
    /* ------------------------------------------------------------------ */
    /*
    ADC_Config_t adc_intr_cfg = {
        .Resolution     = Resolution_12bit,
        .ModeContinuity = Continuous_Mode,
        .ModeSingleScan = Single_Mode,
        .DMA            = DMA_Disabled,
        .InterruptMode  = Interrupt_Enable,
        .NumChannels    = 1,
    };
    ADC_Init(ADC1, &adc_intr_cfg);
    ADC_Channel_Init(ADC1, &ch1_cfg);
    ADC_SetSequenceLength(ADC1, 1);
    ADC_StartConversion(ADC1);
    */

    /* ------------------------------------------------------------------ */
    /* Loop — Example A                                                     */
    /* ------------------------------------------------------------------ */
    while (1)
    {
        // trigger one conversion and wait for it
        ADC_StartConversion(ADC1);
        while (!ADC_EOC(ADC1));

        uint16_t raw = ADC1->DR;            // 0–4095

        // map to delay: min ~5k cycles (full right) to ~500k (full left)
        uint32_t blink_delay = 5000 + ((uint32_t)raw * 120);

        GPIO_TogglePin(GPIOD, 12);
        delay(blink_delay);

        /* ---- Example B threshold check (works alongside interrupt) ---- */
        /*
        if (g_adc_result > 2048)            // above half scale (~1.65V)
            GPIO_WritePin(GPIOD, 14, 1);
        else
            GPIO_WritePin(GPIOD, 14, 0);
        */
    }
}