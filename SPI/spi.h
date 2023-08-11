#ifndef __SPI_H
#define __SPI_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"

/* 函数声明 */
void SPI2_Init(void);                           /* 初始化SPI2 */
void spi2_set_speed(uint32_t speed);            /* 设置SPI2通信波特率 */
uint8_t spi2_read_write_byte(uint8_t txdata);   /* SPI2读写一字节数据 */

#endif
