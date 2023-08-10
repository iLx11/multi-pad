#ifndef __SPI_H
#define __SPI_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"

void SPI2_Init(void);			 //��ʼ��SPI��
void SPI2_SetSpeed(uint8_t SpeedSet); //����SPI�ٶ�
uint8_t SPI2_ReadWriteByte(uint8_t TxData);//SPI���߶�дһ���ֽ�

extern SPI_HandleTypeDef hspi;

#endif

