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
uint8_t spi2_transmit_receive_byte(uint8_t byte_data);
uint8_t spi2_read_write_byte(uint8_t txdata);

#endif //MULTI_SPI2_H
