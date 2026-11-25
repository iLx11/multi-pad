#ifndef MULTIPAD_TEST_HAL_H
#define MULTIPAD_TEST_HAL_H

#include <stdint.h>
uint32_t HAL_GetTick(void);

/* GPIO surface used by the real encoder3 poller. */
#define __weak
typedef struct GPIO_TypeDef GPIO_TypeDef;
typedef struct { uint32_t Pin, Mode, Pull, Speed; } GPIO_InitTypeDef;
#define GPIOD ((GPIO_TypeDef *)0)
#define GPIO_PIN_4 (1u << 4)
#define GPIO_PIN_5 (1u << 5)
#define GPIO_PIN_10 (1u << 10)
#define GPIO_PIN_14 (1u << 14)
#define GPIO_MODE_INPUT 0u
#define GPIO_PULLUP 1u
#define GPIO_SPEED_FREQ_HIGH 3u
#define __HAL_RCC_GPIOD_CLK_ENABLE() ((void)0)
uint8_t HAL_GPIO_ReadPin(GPIO_TypeDef *port, uint16_t pin);
void HAL_GPIO_Init(GPIO_TypeDef *port, GPIO_InitTypeDef *init);

#endif
