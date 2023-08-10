#include "spi111.h"

SPI_HandleTypeDef hspi;

void MX_SPI2_Init(void) {
    // 设置 SPI2
    hspi.Instance = SPI2;
    // 设置SPI工作模式:设置为主SPI
    hspi.Init.Mode = SPI_MODE_MASTER;
    // 设置全双工或是半双工的数据模式
    hspi.Init.Direction = SPI_DIRECTION_2LINES;
    // 设置 SPI 的数据大小，发送8位帧结构
    hspi.Init.DataSize = SPI_DATASIZE_8BIT;
    // 串行同步时钟空闲状态为高电平
    hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;
    // 串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
    // NSS 管脚由硬件还是由软件（SSI） 管理： 内部 NSS 信号有 SSI 位控制
    hspi.Init.NSS = SPI_NSS_SOFT;
    // 定义波特率预分频的之： 波特率预分频值为256
    hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    // 指定数据传输从 MSB 开始还是 LSB 开始： 数据从 MSB 开始
    hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    // CRC 值计算的多项式
    hspi.Init.CRCCalculation = 7;
    // 根据指定参数初始化外设 SPIX 寄存器
    HAL_SPI_Init(&hspi);
}

void SPI2_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    // SPI2 时钟使能
    __HAL_RCC_SPI2_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /** SPI2 GPIO Configuration
        PB13     ------> SPI2_SCK
        PB14     ------> SPI2_MISO
        PB15     ------> SPI2_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    MX_SPI2_Init();
    SPI2_ReadWriteByte(0xff);//启动传输
}

/*SPI 速度设置函数
SpeedSet:
SPI_BaudRatePrescaler_2   2分频
SPI_BaudRatePrescaler_8   8分频
SPI_BaudRatePrescaler_16  16分频
SPI_BaudRatePrescaler_256 256分频*/

void SPI2_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
    /* 修改SPI2的速度设置 */
    MODIFY_REG(hspi.Instance->CR1, SPI_CR1_BR_Msk, SPI_BaudRatePrescaler);
    /* 使能SPI2外设 */
    __HAL_SPI_ENABLE(&hspi);
}

/*SPIx 读写一个字节
TxData:要写入的字节
返回值:读取到的字节*/
uint8_t SPI2_ReadWriteByte(uint8_t TxData) {
    uint8_t RxData;
    uint32_t retry = 0;
    /* 检查发送缓冲区是否为空 */
    while (__HAL_SPI_GET_FLAG(&hspi, SPI_FLAG_TXE) == RESET) {
        retry++;
        if (retry > 200) return 0;
    }
    /* 发送数据 */
    if (HAL_SPI_Transmit(&hspi, &TxData, 1, HAL_MAX_DELAY) != HAL_OK) {
        return 0;
    }
    retry = 0;
    /* 检查接收缓冲区是否非空 */
    while (__HAL_SPI_GET_FLAG(&hspi, SPI_FLAG_RXNE) == RESET) {
        retry++;
        if (retry > 200) return 0;
    }
    /* 接收数据 */
    if (HAL_SPI_Receive(&hspi, &RxData, 1, HAL_MAX_DELAY) != HAL_OK) {
        return 0;
    }
    return RxData;
}
