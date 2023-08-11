#ifndef __SPI_H
#define __SPI_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"

/* �������� */
void SPI2_Init(void);                           /* ��ʼ��SPI2 */
void spi2_set_speed(uint32_t speed);            /* ����SPI2ͨ�Ų����� */
uint8_t spi2_read_write_byte(uint8_t txdata);   /* SPI2��дһ�ֽ����� */

#endif
