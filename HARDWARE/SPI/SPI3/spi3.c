/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/23 20:59
* @version: 1.0
* @description: 
********************************************************************************/


#include "spi3.h"

// SPI句柄
SPI_HandleTypeDef handle_spi3;

void spi3_init(void) {
    // 设置 SPI3
    handle_spi3.Instance = SPI3;
    // 设置SPI工作模式:设置为主SPI
    handle_spi3.Init.Mode = SPI_MODE_MASTER;
    // 设置全双工或是半双工的数据模式
    handle_spi3.Init.Direction = SPI_DIRECTION_2LINES;
    // 设置 SPI 的数据大小，发送8位帧结构
    handle_spi3.Init.DataSize = SPI_DATASIZE_8BIT;
    // 串行同步时钟空闲状态为高电平
    handle_spi3.Init.CLKPolarity = SPI_POLARITY_LOW;
    // 串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    handle_spi3.Init.CLKPhase = SPI_PHASE_1EDGE;
    // NSS 管脚由硬件还是由软件（SSI） 管理： 内部 NSS 信号有 SSI 位控制
    handle_spi3.Init.NSS = SPI_NSS_SOFT;
    // 定义波特率预分频的之： 波特率预分频值为256
    handle_spi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    // 指定数据传输从 MSB 开始还是 LSB 开始： 数据从 MSB 开始
    handle_spi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
    handle_spi3.Init.TIMode = SPI_TIMODE_DISABLE;
    handle_spi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    // CRC 值计算的多项式
    handle_spi3.Init.CRCPolynomial = 10;
    // 根据指定参数初始化外设 SPIX 寄存器d
    if (HAL_SPI_Init(&handle_spi3) != HAL_OK)
    {
        printf("spi3 init error !");
    }
}

// SPI 底层初始化
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (hspi->Instance == SPI3) {
        // 时钟使能
        __HAL_RCC_SPI3_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_3| GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
    if (hspi->Instance == SPI2) {
        // 时钟使能
        __HAL_RCC_SPI2_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_13;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
    if (hspi->Instance == SPI1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        // 时钟使能
        __HAL_RCC_SPI1_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_5| GPIO_PIN_6| GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/********************************************************************************
* SPI 发送并接受一个字节的数据
********************************************************************************/
uint8_t spi3_transmit_receive_byte(uint8_t byte_data) {
    uint8_t receive_data;
    if (HAL_SPI_TransmitReceive(&handle_spi3, &byte_data, &receive_data, 1, 1000) != HAL_OK) return 0;
    return receive_data;
}