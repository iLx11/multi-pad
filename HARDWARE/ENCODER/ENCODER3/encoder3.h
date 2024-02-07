/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/30 8:01
* @version: 1.0
* @description: 
********************************************************************************/


#ifndef MULTI_ENCODER3_H
#define MULTI_ENCODER3_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"

#define ENCODER3_KEY_DOWN 0
#define ENCODER3_KEY_UP 1

#define ENCODER3_KEY_GPIO GPIOD
#define ENCODER3_KEY_PIN GPIO_PIN_14
#define ENABLE_ENCODER3_KEY_RCC __HAL_RCC_GPIOD_CLK_ENABLE()

#define GET_ENCODER3_KEY_STATE HAL_GPIO_ReadPin(ENCODER3_KEY_GPIO, ENCODER3_KEY_PIN)

void encoder3_init(void);
void encoder3_scan(void);

static void encoder3_key_scan(void);

static void encoder3_rotation_scan(void);

__weak void encoder3_callback(uint8_t callback_value);

__weak void encoder3_key_down_callback(void);

__weak void encoder3_key_up_callback(void);

__weak void encoder3_clockwise_callback(void);

__weak void encoder3_anticlockwise_callback(void);

__weak void encoder3_hold_c_callback(void);

__weak void encoder3_hold_a_callback(void);

#endif //MULTI_ENCODER3_H
