/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/30 0:04
* @version: 1.0
* @description: 
********************************************************************************/


#ifndef MULTI_TIM1_H
#define MULTI_TIM1_H

#include "stm32f1xx_hal.h"
#include <stdio.h>

void tim1_init(void);

uint32_t get_encoder1_counter(void);
void set_encoder1_counter(uint8_t set_value);

#endif //MULTI_TIM1_H
