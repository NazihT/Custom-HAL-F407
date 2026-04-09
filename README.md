# STM32F407 Custom HAL

A lightweight, register-level Hardware Abstraction Layer (HAL) for the STM32F407 microcontroller.

This project  is built from scratch with a focus on performance, transparency, and full control over hardware without relying on vendor HAL libraries.

---

## Design Goals

- Minimal abstraction over registers
- High performance, low overhead
- Strong parameter validation using assertions
- Modular structure (peripheral-by-peripheral)
- Clean and readable embedded C

---

## Core Mechanisms

### Assertion & Error Handling
The driver uses `assert\_param` for runtime validation across all peripherals.

On failure:
- Execution is halted in a `while(1)` loop.
- **Hardware Trap:** A debug indicator (Red LED on PD14) is forced HIGH via direct register access to signal a configuration error.

```c
void assert_failed(uint8_t *file, uint32_t line);
```

### Global Clock Management
The RCC module computes and exports global variables representing the current clock tree frequencies. These are used by other peripherals (Timers, ADC, UART) for precise timing calculations.

```c
extern uint32_t SYSCLK_FREQ;
extern uint32_t HCLK_FREQ;
extern uint32_t PCLK1_FREQ;
extern uint32_t PCLK2_FREQ;
```

---

## Current Support

| Peripheral | Status |
|------------|--------|
| RCC        | ✅ Implemented |
| GPIO       | ✅ Implemented |
| TIMERS     | ✅ Implemented |
| EXTI       | ✅ Implemented |
| ADC        | ✅ Implemented |

---

## Peripheral Drivers

### ⚙️ RCC (Reset and Clock Control)

| Function | Description |
|----------|-------------|
| `RCC\_ClockInit` | Initializes the system clock source and applies prescalers. |
| `RCC\_SetPrescalers` | Updates AHB, APB1, and APB2 prescalers dynamically. |
| `RCC\_UseHSI` / `RCC\_UseHSE` | Manages internal/external clock switching and readiness polling. |

**Configuration Structures**
```c
typedef struct {
    AHBPSC_t AHBPSC;
    APBPSC_t APB1PSC;
    APBPSC_t APB2PSC;
} RCC_ConfigPrescaler_t;

typedef struct {
    RCC_ConfigPrescaler_t prescalers;
    ClockSource_t         ClockSource;
} RCC_ConfigClock_t;
```

---

###  GPIO (General Purpose I/O)

| Function | Description |
|----------|-------------|
| `GPIO\_Init` | Configures a GPIO pin mode, speed, and pull-up/down settings. |
| `GPIO\_WritePin` | Atomic Set/Reset of a pin state using the BSRR register. |
| `GPIO\_TogglePin` | Inverts the current pin logic level. |
| `GPIO\_ReadPin` | Returns the logic state of the Input Data Register (IDR). |

**Configuration Structure**
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

---

###  Timer (TIM2 - TIM7)

| Function | Description |
|----------|-------------|
| `TIM\_Init` | Sets up the time-base and operational mode (Basic or PWM). |
| `TIM\_Start` / `TIM\_Stop` | Toggles the CEN bit in the CR1 register. |
| `TIM\_PWM\_SetDutyCycle` | Updates CCRx registers based on 0-100% input. |
| `TIM\_EnableInterrupt` | Enables the UIE bit and configures the corresponding NVIC IRQ. |

**Configuration Structure**
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

---

###  EXTI (External Interrupts)

| Function | Description |
|----------|-------------|
| `EXTI\_Enable` | Complete setup for edge detection, port mapping, and unmasking. |
| `EXTI\_SetEdge` | Configures trigger polarity (Rising, Falling, or Both). |
| `EXTI\_ConfigSYSCFG` | Maps a specific GPIO Port (A-H) to an EXTI line via SYSCFG. |

**Configuration Structure**
```c
typedef struct {
    uint8_t EXTI_n;
    Edge_t  Edge;
    Port_t  Port;
} EXTI_Config_t;
```

---

###  ADC (Analog to Digital Converter)

| Function | Description |
|----------|-------------|
| `ADC\_Init` | Configures resolution, continuity, scan mode, and DMA/Interrupt states. |
| `ADC\_Channel\_Init` | Maps specific channels to the sequence (SQRx) and sets sample time. |
| `ADC\_StartConversion` | Triggers the SWSTART bit to begin a conversion sequence. |
| `ADC\_EOC` | Polls the End Of Conversion flag in the Status Register (SR). |

**Configuration Structures**
```c
typedef struct {
    ADC_Resolution      Resolution;
    ADC_Mode_Continuity ModeContinuity;
    ADC_Mode_SingleScan ModeSingleScan;
    ADC_Mode_Interrupt  InterruptMode;
    ADC_Disc_Num        NumChannels; // For Discontinuous mode
    ADC_DMA_Enable_t    DMA;
} ADC_Config_t;

typedef struct {
    ADC_Channel   Chx;
    ADC_NbSamples SMPx;
    uint8_t       SQx; // Sequence position (1-16)
} ADC_Channel_Config_t;
```

---

## Project Structure

```text
/Core
/src
     RCC_HAL.c
     GPIO_HAL.c
     TIMER_HAL.c
     EXTI_HAL.c
     ADC_HAL.c
/inc
     RCC_HAL.h
     GPIO_HAL.h
     TIMER_HAL.h
     EXTI_HAL.h
     ADC_HAL.h
/Examples
```
