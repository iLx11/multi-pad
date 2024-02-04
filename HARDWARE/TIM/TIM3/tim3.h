/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/30 0:04
* @version: 1.0
* @description: 
********************************************************************************/


#ifndef MULTI_TIM3_H
#define MULTI_TIM3_H

#include "stm32f1xx_hal.h"
#include <stdio.h>

void tim3_init(void);

uint32_t get_encoder2_counter(void);
void set_encoder2_counter(uint8_t set_value);

#endif //MULTI_TIM4_H
