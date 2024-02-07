/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/23 20:59
* @version: 1.0
* @description: 
********************************************************************************/


#ifndef MULTI_SPI1_H
#define MULTI_SPI1_H

#include "stm32f1xx_hal.h"
#include <stdio.h>


/* SPI总线速度设置 */
#define SPI_SPEED_2         0
#define SPI_SPEED_4         1
#define SPI_SPEED_8         2
#define SPI_SPEED_16        3
#define SPI_SPEED_32        4
#define SPI_SPEED_64        5
#define SPI_SPEED_128       6
#define SPI_SPEED_256       7

void spi1_init(void);
HAL_SPI_StateTypeDef spi1_transmit_data(uint8_t* byte_data, uint16_t size);
HAL_SPI_StateTypeDef spi1_receive_data(uint8_t* receive_data, uint16_t size);
uint8_t spi1_transmit_receive_byte(uint8_t transmit_byte);
void spi1_set_speed(uint8_t speed);
#endif //MULTI_SPI1_H
