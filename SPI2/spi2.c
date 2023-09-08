#include "spi2.h"

// SPI句柄
SPI_HandleTypeDef handle_spi2;

// 初始化 SPI
void SPI2_Init(void) {
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
    HAL_SPI_Init(&handle_spi2);
}

// HAL库SPI2初始化MSP函数
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (hspi->Instance == SPI2) {
        // 时钟使能
        __HAL_RCC_SPI2_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /** SPI2 GPIO Configuration
            PB13     ------> SPI2_SCK
            PB14     ------> SPI2_MISO
            PB15     ------> SPI2_MOSI
        */
        GPIO_InitStruct.Pin = OLED_42_SPI_CLK_PIN | OLED_42_SPI_MOSI_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(OLED_42_SPI_GPIO, &GPIO_InitStruct);
    }
    if (hspi->Instance == SPI1) {
        // 时钟使能
        __HAL_RCC_SPI1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }

    if (hspi->Instance == SPI3) {
        // 时钟使能
        __HAL_RCC_SPI3_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}



uint8_t spi2_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;

    if (HAL_SPI_TransmitReceive(&handle_spi2, &txdata, &rxdata, 1, 1000) != HAL_OK)
    {
        return 0;
    }

    return rxdata;
}