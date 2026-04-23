#include "I2C_HAL.h"
#include "UART_HAL.h"
#include "GPIO_HAL.h"

/*
 * I2C Example - STM32F407VG Discovery
 *
 * Reads temperature and humidity from an AM2320 sensor over I2C1,
 * streams results over USART2 at 115200 baud.
 *
 * I2C1:  PB6 — SCL (AF4, open-drain)
 *        PB7 — SDA (AF4, open-drain)
 *        Standard mode, 100kHz
 *
 * USART2: PA2 — TX (AF7), 115200 8N1
 *
 * SYSCLK = 16MHz (HSI)
 *
 * Wiring:
 *   AM2320 VDD -> 3.3V
 *   AM2320 GND -> GND
 *   AM2320 SCL -> PB6  (4.7k pull-up to 3.3V)
 *   AM2320 SDA -> PB7  (4.7k pull-up to 3.3V)
 */

#define AM2320_ADDR     0x5C

// read cmd: function 0x03, start reg 0x00, read 4 regs
static uint8_t  read_cmd[] = { 0x03, 0x00, 0x04 };
static uint8_t  buf[8];

static void delay(volatile uint32_t n) { while (n--); }

// Returns 1 on success, 0 on failure.
// buf layout after success:
//   [2][3] = humidity raw (tenths of %)
//   [4][5] = temperature raw (tenths of °C, bit15 = sign)
static uint8_t AM2320_Read(uint16_t *raw_temp, uint16_t *raw_hum)
{
    // Wake-up pulse — sensor is asleep, NACK is expected
    I2C_Transmit(I2C1, AM2320_ADDR, NULL, 0);
    delay(1000);    // ~800us at 16MHz

    if (I2C_Transmit(I2C1, AM2320_ADDR, read_cmd, 3) == NACK) return 0;
    delay(2000);    // ~1.5ms for sensor to prepare data

    if (I2C_Receive(I2C1, AM2320_ADDR, 8, buf) == NACK) return 0;

    *raw_hum  = (uint16_t)((buf[2] << 8) | buf[3]);
    *raw_temp = (uint16_t)((buf[4] << 8) | buf[5]);
    return 1;
}

int main(void)
{
    // PB6 — I2C1 SCL
    GPIO_Config_t scl_pin = {
        .Pin   = 6,
        .Mode  = AF,
        .Type  = Open_Drain,
        .Speed = High_Speed,
        .Pull  = Pull_Up,
        .AFx   = AF4,
    };
    GPIO_Init(GPIOB, &scl_pin);

    // PB7 — I2C1 SDA
    GPIO_Config_t sda_pin = {
        .Pin   = 7,
        .Mode  = AF,
        .Type  = Open_Drain,
        .Speed = High_Speed,
        .Pull  = Pull_Up,
        .AFx   = AF4,
    };
    GPIO_Init(GPIOB, &sda_pin);

    // PA2 — USART2 TX
    GPIO_Config_t tx_pin = {
        .Pin   = 2,
        .Mode  = AF,
        .Type  = Push_Pull,
        .Speed = High_Speed,
        .Pull  = No_Pull,
        .AFx   = AF7,
    };
    GPIO_Init(GPIOA, &tx_pin);

    I2C_Config_t i2c_cfg = {
        .speed_mode = I2C_STANDARD_MODE,
        .frequency  = 100,
    };
    I2C_Init(I2C1, &i2c_cfg);

    UART_Config_t uart_cfg = {
        .BaudRate       = 115200,
        .Mode           = UART_MODE_TX,
        .WordLength     = WORD_LENGTH_8BIT,
        .InterruptState = UART_INTERRUPT_DISABLE,
    };
    UART_Init(USART2, &uart_cfg);

    uint16_t raw_temp, raw_hum;

    // "Temp: +00.0 C\r\n"
    char temp_str[] = "Temp: +00.0 C\r\n";
    char hum_str[]  = "Hum:   00.0 %\r\n";

    while (1)
    {
        if (AM2320_Read(&raw_temp, &raw_hum))
        {
            // Temperature — bit 15 is sign
            int16_t temp = (raw_temp & 0x8000)
                         ? -(int16_t)(raw_temp & 0x7FFF)
                         :  (int16_t) raw_temp;

            temp_str[6] = (temp < 0) ? '-' : '+';
            if (temp < 0) temp = -temp;

            temp_str[7] = '0' + (temp / 100) % 10;
            temp_str[8] = '0' + (temp / 10)  % 10;
            // [9] is '.'
            temp_str[10] = '0' + (temp % 10);

            hum_str[6] = '0' + (raw_hum / 100) % 10;
            hum_str[7] = '0' + (raw_hum / 10)  % 10;
            // [8] is '.'
            hum_str[9] = '0' + (raw_hum % 10);

            UART_TransmitString(USART2, temp_str);
            UART_TransmitString(USART2, hum_str);
            UART_TransmitString(USART2, "----------\r\n");
        }
        else
        {
            UART_TransmitString(USART2, "AM2320 read failed\r\n");
        }

        delay(1600000);     // ~1 second at 16MHz
    }
}