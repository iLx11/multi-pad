/********************************************************************************
 * @author: iLx1
 * @email: colorful_ilx1@163.com
 * @date: 2023/11/23 20:59
 * @version: 1.0
 * @description:
 ********************************************************************************/

#include "spi2.h"
#include <stdio.h>
#include "stm32f1xx_hal_dma.h"

static void spi2_dma_init(void);

DMA_HandleTypeDef hdma_spi2_tx;
DMA_HandleTypeDef hdma_spi2_rx;

// SPI句柄
SPI_HandleTypeDef handle_spi2;

void spi2_init(void) {
  // 设置 SPI2
  handle_spi2.Instance = SPI2;
  // 设置SPI工作模式:设置为主SPI
  handle_spi2.Init.Mode = SPI_MODE_MASTER;
  // 设置全双工或是半双工的数据模式
  handle_spi2.Init.Direction = SPI_DIRECTION_1LINE;
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
  // dma 初始化
  spi2_dma_init();
}

static void spi2_dma_init(void) {
  // 配置 DMA 通道
  __HAL_RCC_DMA1_CLK_ENABLE(); //DMA1时钟使能

  // 配置 SPI2 TX DMA
  hdma_spi2_tx.Instance = DMA1_Channel5;
  hdma_spi2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_spi2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_spi2_tx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_spi2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_spi2_tx.Init.Mode = DMA_NORMAL;
  hdma_spi2_tx.Init.Priority = DMA_PRIORITY_HIGH;

  if (HAL_DMA_Init(&hdma_spi2_tx) != HAL_OK)
  {
    printf("DMA TX ERROR\n\r");
  }
  __HAL_LINKDMA(&handle_spi2, hdmatx, hdma_spi2_tx);

  // 配置 SPI2 RX DMA
  hdma_spi2_rx.Instance = DMA1_Channel4;
  hdma_spi2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_spi2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_spi2_rx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_spi2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_spi2_rx.Init.Mode = DMA_NORMAL;
  hdma_spi2_rx.Init.Priority = DMA_PRIORITY_LOW;

  if (HAL_DMA_Init(&hdma_spi2_rx) != HAL_OK)
  {
    printf("DMA RX ERROR\n\r");
  }
  __HAL_LINKDMA(&handle_spi2, hdmarx, hdma_spi2_rx);

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}


void DMA1_Channel4_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_spi2_rx);
}

void DMA1_Channel5_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_spi2_tx);
}

HAL_StatusTypeDef spi2_trans_byte(uint8_t byte) {
  // return HAL_SPI_Transmit(&handle_spi2, &byte, 1, 1000);
  return HAL_SPI_Transmit_DMA(&handle_spi2, &byte, 1);
}

HAL_StatusTypeDef spi2_trans_data(uint8_t* data, uint16_t size) {
  return HAL_SPI_Transmit(&handle_spi2, data, size, 1000);
  // return HAL_SPI_Transmit_DMA(&handle_spi2, data, size);
}
