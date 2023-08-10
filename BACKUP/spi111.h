#ifndef __SPI_H
#define __SPI_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"

void SPI2_Init(void);			 //初始化SPI口
void SPI2_SetSpeed(uint8_t SpeedSet); //设置SPI速度
uint8_t SPI2_ReadWriteByte(uint8_t TxData);//SPI总线读写一个字节

extern SPI_HandleTypeDef hspi;

#endif

