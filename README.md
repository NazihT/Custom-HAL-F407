# STM32F4 Custom Register-Level HAL

A high-performance, from-scratch Hardware Abstraction Layer (HAL) for the **STM32F4** series. This library replaces standard bloated libraries with lean, register-direct C code, prioritizing deterministic behavior and hardware safety.



---

## 🛠 Project Philosophy
Most HALs are either too abstracted (hiding the hardware) or too "brittle" (hardcoded values). This project aims for the middle ground: **Readability without Overhead.**

* **Zero Dependencies:** No CubeMX, no Standard Peripheral Library. Just CMSIS headers and the datasheet.
* **Dynamic Clock Synchronization:** A central RCC module tracks `SYSCLK`, `HCLK`, and `PCLK`. Peripheral calculations (Baud rate, Prescalers) update dynamically based on the current clock state.
* **Safety-First I2C:** Includes software-defined timeouts and ACK-Failure (AF) handling to prevent the CPU from hanging on a busy or disconnected bus.

---

## 🚀 Key Modules
| Module | Features |
| :--- | :--- |
| **RCC** | Dynamic clock tree tracking, PLL configuration, and bus gating. |
| **GPIO** | Easy Pin/Mode/Speed/Alternate Function (AF) mapping. |
| **UART/USART** | Auto-calculated Baud Rate Registers (BRR) and Interrupt-driven RX/TX. |
| **I2C** | Non-blocking Master mode with robust error flag monitoring. |
| **DMA** | Stream and Channel management for high-speed data transfer. |
| **Timers** | PWM generation and precise time-base interrupts. |

---

## 💻 Usage Example: UART Initialization
The library uses a descriptive configuration structure. Note that the Baud Rate calculation is decoupled from the hardware—it queries the global clock state automatically.

```c
// 1. Define the UART Configuration
UART_Config_t usart2 = {
    .BaudRate       = 38400,
    .interrupt_mode = RXNE_Interrupt_Enable,
    .Usage          = TX_RX,
    .wordlength     = _8_bit
};

// 2. Define the GPIO Pins (AF7 for USART2 on STM32F4)
GPIO_Config_t tx = { .Pin = 2, .Mode = AF, .AFx = AF7 };
GPIO_Config_t rx = { .Pin = 3, .Mode = AF, .AFx = AF7 };

void uart_init() {
    GPIO_Init(GPIOA, &rx);
    GPIO_Init(GPIOA, &tx);
    
    // The driver uses global PCLK frequency to set the BRR register
    UART_Init(USART2, &usart2);
}
```
## 🛡 Robustness & Error Handling

### **Non-Hanging I2C**
Standard "while loop" flag checking can brick a system if a slave device fails. This HAL implements:
* **Timeout Counters:** Every loop has a maximum iteration count.
* **AF Detection:** If an Acknowledge Failure occurs, the HAL clears the flags and exits gracefully.



### **Clock-Aware Peripherals**
The RCC module maintains global variables for bus speeds. If you change the PLL multiplier to overclock the system, the UART `BRR` calculation adapts immediately using the formula:

$$USART\_BRR = \frac{f_{PCLK}}{8 \times (2 - OVER8) \times BaudRate}$$

---

## 📂 Project Stats
* **Lines of Code:** 1500+ lines of custom Embedded C.
* **Memory Footprint:** Significantly lower than ST-HAL/LL.
* **Target:** STM32F4 (Cortex-M4).

---

*This project was developed for educational purposes to master ARM architecture and register-level peripheral interfacing.*
