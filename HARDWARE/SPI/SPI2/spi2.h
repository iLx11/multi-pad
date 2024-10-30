/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/23 20:59
* @version: 1.0
* @description: 
********************************************************************************/


#ifndef MULTI_SPI2_H
#define MULTI_SPI2_H

#include "stm32f1xx_hal.h"

void spi2_init(void);

HAL_StatusTypeDef spi2_trans_byte(uint8_t byte);

HAL_StatusTypeDef spi2_trans_data(uint8_t* data, uint16_t size);

#endif //MULTI_SPI2_H
