#ifndef EC11_H

#define EC11_H

#include  "stm32f1xx.h"
#include "system_stm32f1xx.h"
#include "oled.h"
#include <stdio.h>

#define  once_angel         18//1个定位点转的度数
#define  point              20//一圈有20个定位点
#define  real_point          5  //实际转几个定位点产生事件
#define EC11_GPIO_RCC        RCC_APB2Periph_GPIOC

#define EC11_GPIO                GPIOC
#define GET_ENCODE_STATE(GPIO) HAL_GPIO_ReadPin(EC11_GPIO,GPIO)

#define EC11_A0_PIN            GPIO_PIN_0
#define EC11_B0_PIN            GPIO_PIN_1

#define EC11_A1_PIN            GPIO_PIN_5
#define EC11_B1_PIN            GPIO_PIN_6

#define EC11_A2_PIN            GPIO_PIN_7
#define EC11_B2_PIN            GPIO_PIN_8

#define EC11_A3_PIN            GPIO_PIN_10
#define EC11_B3_PIN            GPIO_PIN_11

#define EC11_A4_PIN            GPIO_PIN_12
#define EC11_B4_PIN            GPIO_PIN_13

#define HIGH              1
#define LOW               0

#define CW                1//顺时针
#define CWW               0//逆时针

extern void encoder_callback(u_int8_t encoder_index);

void EC11_EXTI_Init();

#endif