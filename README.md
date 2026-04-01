# STM32F407 Custom HAL

A lightweight, register-level Hardware Abstraction Layer (HAL) for the STM32F407 microcontroller.

This project is built from scratch with a focus on performance, transparency, and full control over hardware without relying on vendor HAL libraries.

---

## Design Goals

- Minimal abstraction over registers
- High performance, low overhead
- Strong parameter validation using assertions
- Modular structure (peripheral-by-peripheral)
- Clean and readable embedded C

---

## Current Support

| Peripheral | Status |
|------------|--------|
| GPIO       | ✅ Implemented |

---

## GPIO Driver

### Features

- Pin configuration (Mode, Speed, Output Type, Pull-up/down)
- Alternate Function configuration
- Atomic pin write using BSRR
- Pin toggle and read operations
- Peripheral clock enable abstraction
- Assertion-based parameter validation

---

### API Overview

#### Initialization

```c
HAL_Status_t GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_Config_t *cfg);
```

Configures a GPIO pin using a structured configuration.

---

#### Configuration Functions

```c
void GPIO_PinMode(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Mode_t mode);
void GPIO_SetAFmode(GPIO_TypeDef *GPIOx, uint8_t pin, AlternateFunction_t AFx);
void GPIO_PupPdown(GPIO_TypeDef *GPIOx, uint8_t pin, Pup_Pdown resistor);
void GPIO_Output_Type(GPIO_TypeDef *GPIOx, uint8_t pin, OutputType type);
void GPIO_Output_Speed(GPIO_TypeDef *GPIOx, uint8_t pin, Speed speed);
```

---

#### I/O Operations

```c
void    GPIO_WritePin(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t state);
void    GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint8_t pin);
uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t pin);
```

---

#### Clock Control

```c
void GPIO_EnableClock(GPIO_TypeDef *GPIOx);
```

---

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

---

### Assertion Mechanism

The driver uses `assert_param` for runtime validation.

On failure:
- Execution is halted
- A debug indicator (PD14) is turned on

```c
void assert_failed(uint8_t *file, uint32_t line);
```

This behavior can be overridden by redefining the weak function.

---

## Project Structure

```
/Core
/Drivers
    /GPIO
        GPIO_HAL.c
        GPIO_HAL.h
/Examples
```

---

## Usage

Examples are available in the `/Examples` directory.

---

## Notes

- This HAL is intended for learning and low-level control
- Not a drop-in replacement for vendor HAL
- Designed for STM32F407 (adaptation needed for other MCUs)

