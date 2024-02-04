#ifndef LEDEN_SPI1_H
#define LEDEN_SPI1_H


#include "stm32f1xx_hal.h"

void SPI1_Init(void);                           /* ��ʼ��SPI2 */
void spi1_set_speed(uint32_t speed);            /* ����SPI2ͨ�Ų����� */
uint8_t spi1_read_write_byte(uint8_t txdata);   /* SPI2��дһ�ֽ����� */

#endif //LEDEN_SPI_H
