#include "spi.h"

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
    handle_spi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
    // 串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    handle_spi2.Init.CLKPhase = SPI_PHASE_2EDGE;
    // NSS 管脚由硬件还是由软件（SSI） 管理： 内部 NSS 信号有 SSI 位控制
    handle_spi2.Init.NSS = SPI_NSS_SOFT;
    // 定义波特率预分频的之： 波特率预分频值为256
    handle_spi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    // 指定数据传输从 MSB 开始还是 LSB 开始： 数据从 MSB 开始
    handle_spi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    // CRC 值计算的多项式
    handle_spi2.Init.CRCCalculation = 7;
    // 根据指定参数初始化外设 SPIX 寄存器
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
        GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_15 | GPIO_PIN_14;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

/**
 * @brief   设置SPI2通信波特率
 * @param   speed: SPI2波特率分频系数
 * @arg     SPI_BAUDRATEPRESCALER_2: 2分频
 * @arg     SPI_BAUDRATEPRESCALER_4: 2分频
 * @arg     SPI_BAUDRATEPRESCALER_8: 8分频
 * @arg     SPI_BAUDRATEPRESCALER_16: 16分频
 * @arg     SPI_BAUDRATEPRESCALER_32: 32分频
 * @arg     SPI_BAUDRATEPRESCALER_64: 64分频
 * @arg     SPI_BAUDRATEPRESCALER_128: 128分频
 * @arg     SPI_BAUDRATEPRESCALER_256: 256分频
 * @retval  无
 */
void spi2_set_speed(uint32_t speed)
{
    __HAL_SPI_DISABLE(&handle_spi2);
    handle_spi2.Instance->CR1 &= ~SPI_CR1_BR_Msk;
    handle_spi2.Instance->CR1 |= speed;
    __HAL_SPI_ENABLE(&handle_spi2);
}
/**
 * @brief   SPI2读写一字节数据
 * @param   txdata: 待写入的一字节数据
 * @retval  读取到的一字节数据
 */
uint8_t spi2_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;

    if (HAL_SPI_TransmitReceive(&handle_spi2, &txdata, &rxdata, 1, 1000) != HAL_OK)
    {
        return 0;
    }

    return rxdata;
}