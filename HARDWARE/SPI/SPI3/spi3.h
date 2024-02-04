#ifndef LEDEN_SPI3_H
#define LEDEN_SPI3_H
#include "stm32f1xx_hal.h"

void SPI3_Init(void);                           /* ��ʼ��SPI2 */
void spi3_set_speed(uint32_t speed);            /* ����SPI2ͨ�Ų����� */
uint8_t spi3_read_write_byte(uint8_t txdata);   /* SPI2��дһ�ֽ����� */


#endif //LEDEN_SPI3_H
