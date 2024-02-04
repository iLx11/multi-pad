#include "spi3.h"


// SPI���
SPI_HandleTypeDef handle_spi3;

// ��ʼ�� SPI
void SPI3_Init(void) {
    // ���� SPI2
    handle_spi3.Instance = SPI1;
    // ����SPI����ģʽ:����Ϊ��SPI
    handle_spi3.Init.Mode = SPI_MODE_MASTER;
    // ����ȫ˫�����ǰ�˫��������ģʽ
    handle_spi3.Init.Direction = SPI_DIRECTION_2LINES;
    // ���� SPI �����ݴ�С������8λ֡�ṹ
    handle_spi3.Init.DataSize = SPI_DATASIZE_8BIT;
    // ����ͬ��ʱ�ӿ���״̬Ϊ�ߵ�ƽ
    handle_spi3.Init.CLKPolarity = SPI_POLARITY_HIGH;
    // ����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    handle_spi3.Init.CLKPhase = SPI_PHASE_2EDGE;
    // NSS �ܽ���Ӳ�������������SSI�� ���� �ڲ� NSS �ź��� SSI λ����
    handle_spi3.Init.NSS = SPI_NSS_SOFT;
    // ���岨����Ԥ��Ƶ��֮�� ������Ԥ��ƵֵΪ256
    handle_spi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    // ָ�����ݴ���� MSB ��ʼ���� LSB ��ʼ�� ���ݴ� MSB ��ʼ
    handle_spi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
    handle_spi3.Init.TIMode = SPI_TIMODE_DISABLE;
    handle_spi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    // CRC ֵ����Ķ���ʽ
    handle_spi3.Init.CRCPolynomial = 10;
    // ����ָ��������ʼ������ SPIX �Ĵ���d
    HAL_SPI_Init(&handle_spi3);
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
void spi3_set_speed(uint32_t speed)
{
    __HAL_SPI_DISABLE(&handle_spi3);
    handle_spi3.Instance->CR1 &= ~SPI_CR1_BR_Msk;
    handle_spi3.Instance->CR1 |= speed;
    __HAL_SPI_ENABLE(&handle_spi3);
}
/**
 * @brief   SPI2��дһ�ֽ�����
 * @param   txdata: ��д���һ�ֽ�����
 * @retval  ��ȡ����һ�ֽ�����
 */
uint8_t spi3_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;

    if (HAL_SPI_TransmitReceive(&handle_spi3, &txdata, &rxdata, 1, 1000) != HAL_OK)
    {
        return 0;
    }

    return rxdata;
}