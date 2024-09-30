/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/23 20:59
* @version: 1.0
* @description: 
********************************************************************************/


#include <stdio.h>
#include "spi2.h"

// SPI句柄
SPI_HandleTypeDef handle_spi2;

void spi2_init(void) {
    // 设置 SPI2
    handle_spi2.Instance = SPI2;
    // 设置SPI工作模式:设置为主SPI
    handle_spi2.Init.Mode = SPI_MODE_MASTER;
    // 设置全双工或是半双工的数据模式
    handle_spi2.Init.Direction = SPI_DIRECTION_2LINES;
    // 设置 SPI 的数据大小，发送8位帧结构
    handle_spi2.Init.DataSize = SPI_DATASIZE_8BIT;
    // 串行同步时钟空闲状态为高电平
    handle_spi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    // 串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    handle_spi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    // NSS 管脚由硬件还是由软件（SSI） 管理： 内部 NSS 信号有 SSI 位控制
    handle_spi2.Init.NSS = SPI_NSS_SOFT;
    // 定义波特率预分频的之： 波特率预分频值为256
    handle_spi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    // 指定数据传输从 MSB 开始还是 LSB 开始： 数据从 MSB 开始
    handle_spi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    handle_spi2.Init.TIMode = SPI_TIMODE_DISABLE;
    handle_spi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    // CRC 值计算的多项式
    handle_spi2.Init.CRCPolynomial = 10;
    // 根据指定参数初始化外设 SPIX 寄存器d
    if (HAL_SPI_Init(&handle_spi2) != HAL_OK) {
        printf("spi2 init error !");
    }
}

/********************************************************************************
* SPI 发送并接受一个字节的数据
********************************************************************************/
uint8_t spi2_transmit_receive_byte(uint8_t byte_data) {
    uint8_t receive_data;
    if (HAL_SPI_TransmitReceive(&handle_spi2, &byte_data, &receive_data, 1, 1000) != HAL_OK) return 0;
    return receive_data;
}

uint8_t spi2_read_write_byte(uint8_t txdata) {
    uint8_t rxdata;
    if (HAL_SPI_TransmitReceive(&handle_spi2, &txdata, &rxdata, 1, 1000) != HAL_OK) {
        return 0;
    }
    return rxdata;
}