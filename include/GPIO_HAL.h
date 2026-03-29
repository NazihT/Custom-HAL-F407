#ifndef GPIO_HAL_H
#define GPIO_HAL_H
#include <stddef.h>
#include "stm32f407xx.h"
#define USE_FULL_ASSERT

/* ---- assert_param ---- */
#ifdef USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t *file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

/* ---- validation macros ---- */
#define IS_GPIO_PORT(port)    ((port) == GPIOA || (port) == GPIOB || (port) == GPIOC || \
                               (port) == GPIOD || (port) == GPIOE || (port) == GPIOF || \
                               (port) == GPIOG || (port) == GPIOH || (port) == GPIOI)
#define IS_GPIO_PIN(pin)      ((pin) <= 15)
#define IS_GPIO_MODE(mode)    ((mode) <= ANALOG)
#define IS_GPIO_PULL(pull)    ((pull) <= Pull_Down)
#define IS_GPIO_SPEED(speed)  ((speed) <= VeryHigh_Speed)
#define IS_GPIO_OTYPE(type)   ((type) <= Open_Drain)
#define IS_GPIO_AF(af)        ((af) <= AF15)
#define IS_GPIO_STATE(state)  ((state) <= 1)

/* ---- types ---- */
typedef enum {
    HAL_OK    = 0,
    HAL_ERROR = 1
} HAL_Status_t;

typedef enum {
    INPUT  = 0,
    OUTPUT = 1,
    AF     = 2,
    ANALOG = 3
} GPIO_Mode_t;

typedef enum {
    Push_Pull  = 0,
    Open_Drain = 1
} OutputType;

typedef enum {
    No_Pull   = 0,
    Pull_Up   = 1,
    Pull_Down = 2
} Pup_Pdown;

typedef enum {
    Low_Speed      = 0,
    Medium_Speed   = 1,
    High_Speed     = 2,
    VeryHigh_Speed = 3
} Speed;

typedef enum {
    AF0,  AF1,  AF2,  AF3,
    AF4,  AF5,  AF6,  AF7,
    AF8,  AF9,  AF10, AF11,
    AF12, AF13, AF14, AF15
} AlternateFunction_t;

typedef struct {
    uint8_t             Pin;
    GPIO_Mode_t         Mode;
    OutputType          Type;
    Speed               Speed;
    Pup_Pdown           Pull;
    AlternateFunction_t AFx;
} GPIO_Config_t;

/* ---- API ---- */
HAL_Status_t GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_Config_t *cfg);
void         GPIO_PinMode(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Mode_t mode);
void         GPIO_SetAFmode(GPIO_TypeDef *GPIOx, uint8_t pin, AlternateFunction_t AFx);
void         GPIO_WritePin(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t state);
void         GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint8_t pin);
uint8_t      GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t pin);
void         GPIO_PupPdown(GPIO_TypeDef *GPIOx, uint8_t pin, Pup_Pdown resistor);
void         GPIO_Output_Type(GPIO_TypeDef *GPIOx, uint8_t pin, OutputType type);
void         GPIO_Output_Speed(GPIO_TypeDef *GPIOx, uint8_t pin, Speed speed);
void         GPIO_EnableClock(GPIO_TypeDef *GPIOx);

#endif