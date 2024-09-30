/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/23 20:59
* @version: 1.0
* @description: 
********************************************************************************/


#include "spi1.h"
// SPI句柄
SPI_HandleTypeDef handle_spi1;

void spi1_init(void) {
    __HAL_RCC_SPI1_CLK_ENABLE();

    // 设置 SPI1
    handle_spi1.Instance = SPI1;
    // 设置SPI工作模式:设置为主SPI
    handle_spi1.Init.Mode = SPI_MODE_MASTER;
    // 设置全双工或是半双工的数据模式
    handle_spi1.Init.Direction = SPI_DIRECTION_2LINES;
    // 设置 SPI 的数据大小，发送8位帧结构
    handle_spi1.Init.DataSize = SPI_DATASIZE_8BIT;
    // 串行同步时钟空闲状态为高电平
    handle_spi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
    // 串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    handle_spi1.Init.CLKPhase = SPI_PHASE_2EDGE;
    // NSS 管脚由硬件还是由软件（SSI） 管理： 内部 NSS 信号有 SSI 位控制
    handle_spi1.Init.NSS = SPI_NSS_SOFT;
    // 定义波特率预分频的之： 波特率预分频值为256
    handle_spi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    // 指定数据传输从 MSB 开始还是 LSB 开始： 数据从 MSB 开始
    handle_spi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    handle_spi1.Init.TIMode = SPI_TIMODE_DISABLE;
    handle_spi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    // CRC 值计算的多项式
    handle_spi1.Init.CRCPolynomial = 7;
    // 根据指定参数初始化外设 SPIX 寄存器d
    if (HAL_SPI_Init(&handle_spi1) != HAL_OK) {
        printf("spi1 init error !");
    }
//    __HAL_SPI_ENABLE(&handle_spi1);
}

/********************************************************************************
* SPI 发送数据
********************************************************************************/
HAL_SPI_StateTypeDef spi1_transmit_data(uint8_t* byte_data, uint16_t size) {
    return HAL_SPI_Transmit(&handle_spi1, byte_data, size, 100);
}
/********************************************************************************
* SPI 接收数据
********************************************************************************/
HAL_SPI_StateTypeDef spi1_receive_data(uint8_t* receive_data, uint16_t size) {
    return HAL_SPI_Receive(&handle_spi1, receive_data, size, 100);
}

/********************************************************************************
* SPI 发送单字节数据
********************************************************************************/
uint8_t spi1_transmit_receive_byte(uint8_t transmit_byte) {
    uint8_t receive_data;
    HAL_SPI_TransmitReceive(&handle_spi1, &transmit_byte, &receive_data, 1, 100);
    return receive_data;
}
/********************************************************************************
* 设置速率
********************************************************************************/
void spi1_set_speed(uint8_t speed) {
    assert_param(IS_SPI_BAUDRATE_PRESCALER(speed)); /* 判断有效性 */
    __HAL_SPI_DISABLE(&handle_spi1);             /* 关闭SPI */
    handle_spi1.Instance->CR1 &= 0XFFC7;         /* 位3-5清零，用来设置波特率 */
    handle_spi1.Instance->CR1 |= speed << 3;     /* 设置SPI速度 */
    __HAL_SPI_ENABLE(&handle_spi1);              /* 使能SPI */
}