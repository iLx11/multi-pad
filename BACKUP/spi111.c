#include "spi111.h"

SPI_HandleTypeDef hspi;

void MX_SPI2_Init(void) {
    // ���� SPI2
    hspi.Instance = SPI2;
    // ����SPI����ģʽ:����Ϊ��SPI
    hspi.Init.Mode = SPI_MODE_MASTER;
    // ����ȫ˫�����ǰ�˫��������ģʽ
    hspi.Init.Direction = SPI_DIRECTION_2LINES;
    // ���� SPI �����ݴ�С������8λ֡�ṹ
    hspi.Init.DataSize = SPI_DATASIZE_8BIT;
    // ����ͬ��ʱ�ӿ���״̬Ϊ�ߵ�ƽ
    hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;
    // ����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
    // NSS �ܽ���Ӳ�������������SSI�� ���� �ڲ� NSS �ź��� SSI λ����
    hspi.Init.NSS = SPI_NSS_SOFT;
    // ���岨����Ԥ��Ƶ��֮�� ������Ԥ��ƵֵΪ256
    hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    // ָ�����ݴ���� MSB ��ʼ���� LSB ��ʼ�� ���ݴ� MSB ��ʼ
    hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    // CRC ֵ����Ķ���ʽ
    hspi.Init.CRCCalculation = 7;
    // ����ָ��������ʼ������ SPIX �Ĵ���
    HAL_SPI_Init(&hspi);
}

void SPI2_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    // SPI2 ʱ��ʹ��
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
    SPI2_ReadWriteByte(0xff);//��������
}

/*SPI �ٶ����ú���
SpeedSet:
SPI_BaudRatePrescaler_2   2��Ƶ
SPI_BaudRatePrescaler_8   8��Ƶ
SPI_BaudRatePrescaler_16  16��Ƶ
SPI_BaudRatePrescaler_256 256��Ƶ*/

void SPI2_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
    /* �޸�SPI2���ٶ����� */
    MODIFY_REG(hspi.Instance->CR1, SPI_CR1_BR_Msk, SPI_BaudRatePrescaler);
    /* ʹ��SPI2���� */
    __HAL_SPI_ENABLE(&hspi);
}

/*SPIx ��дһ���ֽ�
TxData:Ҫд����ֽ�
����ֵ:��ȡ�����ֽ�*/
uint8_t SPI2_ReadWriteByte(uint8_t TxData) {
    uint8_t RxData;
    uint32_t retry = 0;
    /* ��鷢�ͻ������Ƿ�Ϊ�� */
    while (__HAL_SPI_GET_FLAG(&hspi, SPI_FLAG_TXE) == RESET) {
        retry++;
        if (retry > 200) return 0;
    }
    /* �������� */
    if (HAL_SPI_Transmit(&hspi, &TxData, 1, HAL_MAX_DELAY) != HAL_OK) {
        return 0;
    }
    retry = 0;
    /* �����ջ������Ƿ�ǿ� */
    while (__HAL_SPI_GET_FLAG(&hspi, SPI_FLAG_RXNE) == RESET) {
        retry++;
        if (retry > 200) return 0;
    }
    /* �������� */
    if (HAL_SPI_Receive(&hspi, &RxData, 1, HAL_MAX_DELAY) != HAL_OK) {
        return 0;
    }
    return RxData;
}
