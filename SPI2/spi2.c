#include "spi2.h"

// SPI���
SPI_HandleTypeDef handle_spi2;

// ��ʼ�� SPI
void SPI2_Init(void) {
    // ���� SPI2
    handle_spi2.Instance = OLED_42_SPI;
    // ����SPI����ģʽ:����Ϊ��SPI
    handle_spi2.Init.Mode = SPI_MODE_MASTER;
    // ����ȫ˫�����ǰ�˫��������ģʽ
    handle_spi2.Init.Direction = SPI_DIRECTION_1LINE;
    // ���� SPI �����ݴ�С������8λ֡�ṹ
    handle_spi2.Init.DataSize = SPI_DATASIZE_8BIT;
    // ����ͬ��ʱ�ӿ���״̬Ϊ�ߵ�ƽ
    handle_spi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    // ����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    handle_spi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    // NSS �ܽ���Ӳ�������������SSI�� ���� �ڲ� NSS �ź��� SSI λ����
    handle_spi2.Init.NSS = SPI_NSS_SOFT;
    // ���岨����Ԥ��Ƶ��֮�� ������Ԥ��ƵֵΪ256
    handle_spi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    // ָ�����ݴ���� MSB ��ʼ���� LSB ��ʼ�� ���ݴ� MSB ��ʼ
    handle_spi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    // CRC ֵ����Ķ���ʽ
    handle_spi2.Init.CRCCalculation = 7;
    // ����ָ��������ʼ������ SPIX �Ĵ���
    HAL_SPI_Init(&handle_spi2);
}

// HAL��SPI2��ʼ��MSP����
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (hspi->Instance == SPI2) {
        // ʱ��ʹ��
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
        // ʱ��ʹ��
        __HAL_RCC_SPI1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /** SPI2 GPIO Configuration
            PB13     ------> SPI2_SCK
            PB14     ------> SPI2_MISO
            PB15     ------> SPI2_MOSI
        */
        GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
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