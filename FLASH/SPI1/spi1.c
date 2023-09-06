#include "spi1.h"

// SPI���
SPI_HandleTypeDef handle_spi1;

// ��ʼ�� SPI
void SPI1_Init(void) {
    // ���� SPI2
    handle_spi1.Instance = SPI1;
    // ����SPI����ģʽ:����Ϊ��SPI
    handle_spi1.Init.Mode = SPI_MODE_MASTER;
    // ����ȫ˫�����ǰ�˫��������ģʽ
    handle_spi1.Init.Direction = SPI_DIRECTION_2LINES;
    // ���� SPI �����ݴ�С������8λ֡�ṹ
    handle_spi1.Init.DataSize = SPI_DATASIZE_8BIT;
    // ����ͬ��ʱ�ӿ���״̬Ϊ�ߵ�ƽ
    handle_spi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
    // ����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    handle_spi1.Init.CLKPhase = SPI_PHASE_2EDGE;
    // NSS �ܽ���Ӳ�������������SSI�� ���� �ڲ� NSS �ź��� SSI λ����
    handle_spi1.Init.NSS = SPI_NSS_SOFT;
    // ���岨����Ԥ��Ƶ��֮�� ������Ԥ��ƵֵΪ256
    handle_spi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    // ָ�����ݴ���� MSB ��ʼ���� LSB ��ʼ�� ���ݴ� MSB ��ʼ
    handle_spi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    // CRC ֵ����Ķ���ʽ
    handle_spi1.Init.CRCCalculation = 7;
    // ����ָ��������ʼ������ SPIX �Ĵ���
    HAL_SPI_Init(&handle_spi1);
}

/**
 * @brief   ����SPI2ͨ�Ų�����
 * @param   speed: SPI2�����ʷ�Ƶϵ��
 * @arg     SPI_BAUDRATEPRESCALER_2: 2��Ƶ
 * @arg     SPI_BAUDRATEPRESCALER_4: 2��Ƶ
 * @arg     SPI_BAUDRATEPRESCALER_8: 8��Ƶ
 * @arg     SPI_BAUDRATEPRESCALER_16: 16��Ƶ
 * @arg     SPI_BAUDRATEPRESCALER_32: 32��Ƶ
 * @arg     SPI_BAUDRATEPRESCALER_64: 64��Ƶ
 * @arg     SPI_BAUDRATEPRESCALER_128: 128��Ƶ
 * @arg     SPI_BAUDRATEPRESCALER_256: 256��Ƶ
 * @retval  ��
 */
void spi1_set_speed(uint32_t speed)
{
    __HAL_SPI_DISABLE(&handle_spi1);
    handle_spi1.Instance->CR1 &= ~SPI_CR1_BR_Msk;
    handle_spi1.Instance->CR1 |= speed;
    __HAL_SPI_ENABLE(&handle_spi1);
}
/**
 * @brief   SPI2��дһ�ֽ�����
 * @param   txdata: ��д���һ�ֽ�����
 * @retval  ��ȡ����һ�ֽ�����
 */
uint8_t spi1_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;

    if (HAL_SPI_TransmitReceive(&handle_spi1, &txdata, &rxdata, 1, 1000) != HAL_OK)
    {
        return 0;
    }

    return rxdata;
}