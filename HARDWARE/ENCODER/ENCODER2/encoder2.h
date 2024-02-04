/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/30 8:01
* @version: 1.0
* @description:
********************************************************************************/


#ifndef MULTI_ENCODER2_H
#define MULTI_ENCODER2_H

#include "stm32f1xx_hal.h"

#define ENCODER2_KEY_DOWN 0
#define ENCODER2_KEY_UP 1

#define ENCODER2_KEY_GPIO GPIOD
#define ENCODER2_KEY_PIN GPIO_PIN_15
#define ENABLE_ENCODER2_KEY_RCC __HAL_RCC_GPIOD_CLK_ENABLE()

#define GET_ENCODER2_KEY_STATE HAL_GPIO_ReadPin(ENCODER2_KEY_GPIO, ENCODER2_KEY_PIN)

void encoder2_init(void);
void encoder2_scan(void);

static void encoder2_key_scan(void);

static void encoder2_rotation_scan(void);

__weak void encoder2_key_down_callback(void);

__weak void encoder2_key_up_callback(void);

__weak void encoder2_clockwise_callback(void);

__weak void encoder2_anticlockwise_callback(void);

__weak void encoder2_hold_c_callback(void);

__weak void encoder2_hold_a_callback(void);

#endif //MULTI_ENCODER2_H
