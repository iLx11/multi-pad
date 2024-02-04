#ifndef LEDEN_SPI3_H
#define LEDEN_SPI3_H
#include "stm32f1xx_hal.h"

void SPI3_Init(void);                           /* 初始化SPI2 */
void spi3_set_speed(uint32_t speed);            /* 设置SPI2通信波特率 */
uint8_t spi3_read_write_byte(uint8_t txdata);   /* SPI2读写一字节数据 */


#endif //LEDEN_SPI3_H
