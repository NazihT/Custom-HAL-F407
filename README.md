# STM32F407 Custom HAL

A lightweight, register-level Hardware Abstraction Layer (HAL) for the STM32F407 microcontroller.
Built from scratch with a focus on performance, transparency, and full hardware control — no vendor HAL bloat.

---

## Design Goals

- Minimal abstraction over registers
- High performance, low overhead
- Strong parameter validation using `assert_param`
- Modular structure — one driver per peripheral
- Clean, readable embedded C

---

## Current Support

| Peripheral | Status         |
|------------|----------------|
| GPIO       | ✅ Implemented |
| Timers     | ✅ Implemented |
| UART       | ✅ Implemented |
| I2C        | ✅ Implemented |
| ADC        | ✅ Implemented |
| EXTI       | ✅ Implemented |

---

## Assertion Mechanism

All drivers use `assert_param` for runtime parameter validation.
Enable it by defining `USE_FULL_ASSERT` in your build flags.

On assertion failure:
- Execution halts in an infinite loop
- PD14 (red LED on the Discovery board) is turned on as a visual indicator

```c
void assert_failed(uint8_t *file, uint32_t line);
```

This is declared `__attribute__((weak))` — override it in your application if needed.
If `USE_FULL_ASSERT` is not defined, all assertions compile away to nothing.

---

## GPIO Driver

### Features
- Pin mode, speed, output type, and pull resistor configuration
- Alternate function mapping
- Atomic pin set/reset via BSRR
- Pin toggle and read
- Clock enable per port

### Configuration Structure
```c
typedef struct {
    uint8_t             Pin;
    GPIO_Mode_t         Mode;
    OutputType          Type;
    Speed               Speed;
    Pup_Pdown           Pull;
    AlternateFunction_t AFx;
} GPIO_Config_t;
```

### Enums
```c
typedef enum { INPUT, OUTPUT, AF, ANALOG } GPIO_Mode_t;
typedef enum { Push_Pull, Open_Drain }     OutputType;
typedef enum { Low_Speed, Medium_Speed, High_Speed, Very_High_Speed } Speed;
typedef enum { No_Pull, Pull_Up, Pull_Down } Pup_Pdown;
typedef enum { AF0, AF1, AF2, ..., AF15 }  AlternateFunction_t;
```

### API
```c
HAL_Status_t GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_Config_t *cfg);

void    GPIO_PinMode(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Mode_t mode);
void    GPIO_SetAFmode(GPIO_TypeDef *GPIOx, uint8_t pin, AlternateFunction_t AFx);
void    GPIO_PupPdown(GPIO_TypeDef *GPIOx, uint8_t pin, Pup_Pdown resistor);
void    GPIO_Output_Type(GPIO_TypeDef *GPIOx, uint8_t pin, OutputType type);
void    GPIO_Output_Speed(GPIO_TypeDef *GPIOx, uint8_t pin, Speed speed);

void    GPIO_WritePin(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t state);
void    GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint8_t pin);
uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t pin);

void    GPIO_EnableClock(GPIO_TypeDef *GPIOx);
```

### Example
```c
GPIO_Config_t led = {
    .Pin   = 13,
    .Mode  = OUTPUT,
    .Type  = Push_Pull,
    .Speed = Low_Speed,
    .Pull  = No_Pull,
};
GPIO_Init(GPIOD, &led);
GPIO_TogglePin(GPIOD, 13);
```

---

## Timer Driver

### Features
- Basic mode with optional update interrupt
- PWM output mode (channels 1–4)
- Prescaler and auto-reload configuration
- One-pulse mode
- Counter read, write, and reset
- NVIC integration

### Configuration Structure
```c
typedef struct {
    Timer_MODE      Mode;
    Interrupt_State InterruptState;
    uint16_t        PSC;
    uint16_t        ARR;
    uint8_t         DefaultDuty_Percent;
    PWM_Channel     CHx;
} TIMER_Config_t;
```

### Enums
```c
typedef enum { TIMER_MODE_BASIC, TIMER_MODE_PWM }              Timer_MODE;
typedef enum { TIMER_INTERRUPT_ENABLE, TIMER_INTERRUPT_DISABLE } Interrupt_State;
typedef enum { CHANNEL_1, CHANNEL_2, CHANNEL_3, CHANNEL_4 }   PWM_Channel;
```

### Supported Timers
`TIM2`, `TIM3`, `TIM4`, `TIM5`, `TIM6`, `TIM7`

### API
```c
void      TIM_Init(TIM_TypeDef *TIMx, TIMER_Config_t *cfg);
void      TIM_Start(TIM_TypeDef *TIMx);
void      TIM_Stop(TIM_TypeDef *TIMx);
void      TIM_EnableInterrupt(TIM_TypeDef *TIMx, Interrupt_State intr);
void      TIM_EnableClock(TIM_TypeDef *TIMx);

void      TIM_PWM_EnableChannel(TIM_TypeDef *TIMx, PWM_Channel channel);
void      TIM_PWM_SetDutyCycle(TIM_TypeDef *TIMx, PWM_Channel channel, uint8_t duty_percent);

uint8_t   TIM_GetFlagStatus(TIM_TypeDef *TIMx);
void      TIM_ClearFlag(TIM_TypeDef *TIMx);
void      TIM_ResetCounter(TIM_TypeDef *TIMx);
uint32_t  TIM_GetCounter(TIM_TypeDef *TIMx);
void      TIM_SetCounter(TIM_TypeDef *TIMx, uint32_t value);
void      TIM_SetPrescaler(TIM_TypeDef *TIMx, uint16_t prescaler);
void      TIM_SetAutoReload(TIM_TypeDef *TIMx, uint32_t auto_reload);
void      TIM_OnePulseMode(TIM_TypeDef *TIMx);
IRQn_Type TIM_GetIRQn(TIM_TypeDef *TIMx);
```

### Example
```c
// TIM2 — 1Hz interrupt
TIMER_Config_t tim2_cfg = {
    .Mode           = TIMER_MODE_BASIC,
    .PSC            = 15999,
    .ARR            = 999,
    .InterruptState = TIMER_INTERRUPT_ENABLE,
};
TIM_Init(TIM2, &tim2_cfg);
TIM_Start(TIM2);

// TIM4 CH1 — PWM on PD12
TIMER_Config_t tim4_cfg = {
    .Mode                = TIMER_MODE_PWM,
    .PSC                 = 15999,
    .ARR                 = 999,
    .CHx                 = CHANNEL_1,
    .DefaultDuty_Percent = 50,
};
TIM_Init(TIM4, &tim4_cfg);
TIM_Start(TIM4);
```

> **Note:** Configure the GPIO pin in AF mode before calling `TIM_Init` for PWM.

---

## UART Driver

### Features
- TX, RX, or TX+RX mode
- 8-bit and 9-bit word length
- RXNE and IDLE interrupt support
- NVIC integration
- Correct BRR calculation — USART1 and USART6 use APB2, all others use APB1

### Configuration Structure
```c
typedef struct {
    uint32_t       BaudRate;
    UART_Mode      Mode;
    UART_Interrupt InterruptState;
    Word_Length    WordLength;
} UART_Config_t;
```

### Enums
```c
typedef enum { UART_MODE_TX, UART_MODE_RX, UART_MODE_TX_RX }        UART_Mode;
typedef enum { UART_INTERRUPT_DISABLE, UART_INTERRUPT_ENABLE }        UART_Interrupt;
typedef enum { WORD_LENGTH_8BIT, WORD_LENGTH_9BIT }                   Word_Length;
```

### Supported Peripherals
`USART1`, `USART2`, `USART3`, `UART4`, `UART5`, `USART6`

### API
```c
void      UART_Init(USART_TypeDef *uartx, UART_Config_t *cfg);
void      UART_EnableClock(USART_TypeDef *uartx);
void      UART_SetMode(USART_TypeDef *uartx, UART_Mode mode);
void      UART_SetWordLength(USART_TypeDef *uartx, Word_Length wordlength);
void      UART_SetInterrupt(USART_TypeDef *uartx, UART_Interrupt intr);
void      UART_Transmit(USART_TypeDef *uartx, uint8_t data);
void      UART_TransmitString(USART_TypeDef *uartx, const char *string);
void      UART_RxClearFlag(USART_TypeDef *uartx);
IRQn_Type UART_GetIRQn(USART_TypeDef *uartx);
```

### Example
```c
UART_Config_t uart_cfg = {
    .BaudRate       = 115200,
    .Mode           = UART_MODE_TX,
    .WordLength     = WORD_LENGTH_8BIT,
    .InterruptState = UART_INTERRUPT_DISABLE,
};
UART_Init(USART2, &uart_cfg);
UART_TransmitString(USART2, "Hello\r\n");
```

> **Note:** Configure the GPIO TX/RX pins in AF mode (AF7 for USART1/2/3, AF8 for UART4/5/USART6) before calling `UART_Init`.

---

## I2C Driver

### Features
- Standard mode (up to 100kHz) and Fast mode (up to 400kHz)
- Blocking transmit with ACK/NACK detection and timeout
- Blocking receive with proper NACK on last byte
- CR2 FREQ field correctly initialized
- Correct TRISE calculation per speed mode

### Configuration Structure
```c
typedef struct {
    I2C_Speed speed_mode;
    uint16_t  frequency;    // in kHz (e.g. 100 = 100kHz, 400 = 400kHz)
} I2C_Config_t;
```

### Enums
```c
typedef enum { I2C_STANDARD_MODE, I2C_FAST_MODE } I2C_Speed;
typedef enum { NACK, ACK }                         ACK_Status;
```

### Supported Peripherals
`I2C1`, `I2C2`, `I2C3`

### API
```c
void       I2C_Init(I2C_TypeDef *i2cx, I2C_Config_t *cfg);
void       I2C_EnableClock(I2C_TypeDef *i2cx);
void       I2C_SetSpeed(I2C_TypeDef *i2cx, I2C_Speed speed_mode, uint16_t frequency);
ACK_Status I2C_Transmit(I2C_TypeDef *i2cx, uint8_t addr, uint8_t *data, uint8_t size);
ACK_Status I2C_Receive(I2C_TypeDef *i2cx, uint8_t addr, uint8_t size, uint8_t *buffer);
```

### Example
```c
I2C_Config_t i2c_cfg = {
    .speed_mode = I2C_STANDARD_MODE,
    .frequency  = 100,
};
I2C_Init(I2C1, &i2c_cfg);

uint8_t cmd[] = { 0x00 };
uint8_t buf[2];

I2C_Transmit(I2C1, 0x5C, cmd, 1);
I2C_Receive(I2C1, 0x5C, 2, buf);
```

> **Note:** SDA and SCL pins must be configured as AF, Open-Drain, with pull-ups. External 4.7kΩ pull-up resistors to VDD are required — the internal weak pull-ups are not sufficient for I2C.

---

## ADC Driver

### Features
- 12-bit, 10-bit, 8-bit, and 6-bit resolution
- Single and continuous conversion modes
- Single and scan sequence modes
- Discontinuous mode with configurable channel grouping
- Configurable sampling time per channel
- Multi-channel sequencer (up to 16 conversions)
- DMA mode support
- EOC interrupt with NVIC integration

### Configuration Structures
```c
typedef struct {
    ADC_Resolution       Resolution;
    ADC_Mode_Continuity  ModeContinuity;
    ADC_Mode_SingleScan  ModeSingleScan;
    ADC_DMA              DMA;
    ADC_Interrupt        InterruptMode;
    uint8_t              NumChannels;
} ADC_Config_t;

typedef struct {
    ADC_Channel    Chx;
    ADC_NbSamples  SMPx;
    uint8_t        SQx;    // position in conversion sequence (1-based)
} ADC_Channel_Config_t;
```

### Enums
```c
typedef enum { Resolution_12bit, Resolution_10bit,
               Resolution_8bit,  Resolution_6bit  } ADC_Resolution;

typedef enum { Continuous_Mode, Discontinuous_Mode } ADC_Mode_Continuity;
typedef enum { Single_Mode, Scan_Mode }              ADC_Mode_SingleScan;
typedef enum { DMA_Disabled, DMA_Enabled }           ADC_DMA;
typedef enum { Interrupt_Disable, Interrupt_Enable } ADC_Interrupt;

typedef enum { ADC_CHANNEL_0, ..., ADC_CHANNEL_15 } ADC_Channel;
typedef enum { SMP_3_Cycles, SMP_15_Cycles, SMP_28_Cycles,
               SMP_56_Cycles, SMP_84_Cycles, SMP_112_Cycles,
               SMP_144_Cycles, SMP_480_Cycles } ADC_NbSamples;
```

### Supported Peripherals
`ADC1`, `ADC2`, `ADC3`

### API
```c
void    ADC_Init(ADC_TypeDef *adcx, ADC_Config_t *cfg);
void    ADC_ClockEnable(ADC_TypeDef *adcx);
void    ADC_SetResolution(ADC_TypeDef *adcx, ADC_Resolution resolution);
void    ADC_SetContinuity(ADC_TypeDef *adcx, ADC_Mode_Continuity mode);
void    ADC_SetSingleScan(ADC_TypeDef *adcx, ADC_Mode_SingleScan mode);
void    ADC_SetSMPx(ADC_TypeDef *adcx, ADC_Channel chx, ADC_NbSamples smp);
void    ADC_SetSequence(ADC_TypeDef *adcx, ADC_Channel chx, uint8_t sequence);
void    ADC_SetSequenceLength(ADC_TypeDef *adcx, uint8_t length);
void    ADC_Channel_Init(ADC_TypeDef *adcx, ADC_Channel_Config_t *cfg);
void    ADC_StartConversion(ADC_TypeDef *adcx);
uint8_t ADC_EOC(ADC_TypeDef *adcx);
```

### Example
```c
ADC_Config_t adc_cfg = {
    .Resolution     = Resolution_12bit,
    .ModeContinuity = Discontinuous_Mode,
    .ModeSingleScan = Single_Mode,
    .DMA            = DMA_Disabled,
    .InterruptMode  = Interrupt_Disable,
    .NumChannels    = 1,
};
ADC_Init(ADC1, &adc_cfg);

ADC_Channel_Config_t ch_cfg = {
    .Chx  = ADC_CHANNEL_1,
    .SMPx = SMP_84_Cycles,
    .SQx  = 1,
};
ADC_Channel_Init(ADC1, &ch_cfg);
ADC_SetSequenceLength(ADC1, 1);

ADC_StartConversion(ADC1);
while (!ADC_EOC(ADC1));
uint16_t result = ADC1->DR;
```

> **Note:** Configure the analog input pin in `ANALOG` mode with no pull resistors before calling `ADC_Init`.

---

## Notes

- Designed specifically for the **STM32F407VG** — adaptation required for other MCUs
- SYSCLK assumed to be **16MHz (HSI)** in all examples — update BRR, PSC, ARR, and delay loops if you run a different clock
- Not a drop-in replacement for the vendor HAL — this is intentional
- Built for learning, low-level control, and projects where every cycle counts
