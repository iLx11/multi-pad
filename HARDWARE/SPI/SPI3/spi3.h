/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/23 20:59
* @version: 1.0
* @description: 
********************************************************************************/


#ifndef MULTI_SPI3_H
#define MULTI_SPI3_H

#include <stdio.h>
#include "stm32f1xx_hal.h"

void spi3_init(void);
uint8_t spi3_transmit_receive_byte(uint8_t byte_data);

#endif //MULTI_SPI3_H
