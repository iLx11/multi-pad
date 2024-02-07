/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/23 20:59
* @version: 1.0
* @description: 
********************************************************************************/


#include "spi1.h"
// SPI���
SPI_HandleTypeDef handle_spi1;

void spi1_init(void) {
    __HAL_RCC_SPI1_CLK_ENABLE();

    // ���� SPI1
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
    handle_spi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    // ָ�����ݴ���� MSB ��ʼ���� LSB ��ʼ�� ���ݴ� MSB ��ʼ
    handle_spi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    handle_spi1.Init.TIMode = SPI_TIMODE_DISABLE;
    handle_spi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    // CRC ֵ����Ķ���ʽ
    handle_spi1.Init.CRCPolynomial = 7;
    // ����ָ��������ʼ������ SPIX �Ĵ���d
    if (HAL_SPI_Init(&handle_spi1) != HAL_OK) {
        printf("spi1 init error !");
    }
//    __HAL_SPI_ENABLE(&handle_spi1);
}

/********************************************************************************
* SPI ��������
********************************************************************************/
HAL_SPI_StateTypeDef spi1_transmit_data(uint8_t* byte_data, uint16_t size) {
    return HAL_SPI_Transmit(&handle_spi1, byte_data, size, 100);
}
/********************************************************************************
* SPI ��������
********************************************************************************/
HAL_SPI_StateTypeDef spi1_receive_data(uint8_t* receive_data, uint16_t size) {
    return HAL_SPI_Receive(&handle_spi1, receive_data, size, 100);
}

/********************************************************************************
* SPI ���͵��ֽ�����
********************************************************************************/
uint8_t spi1_transmit_receive_byte(uint8_t transmit_byte) {
    uint8_t receive_data;
    HAL_SPI_TransmitReceive(&handle_spi1, &transmit_byte, &receive_data, 1, 100);
    return receive_data;
}
/********************************************************************************
* ��������
********************************************************************************/
void spi1_set_speed(uint8_t speed) {
    assert_param(IS_SPI_BAUDRATE_PRESCALER(speed)); /* �ж���Ч�� */
    __HAL_SPI_DISABLE(&handle_spi1);             /* �ر�SPI */
    handle_spi1.Instance->CR1 &= 0XFFC7;         /* λ3-5���㣬�������ò����� */
    handle_spi1.Instance->CR1 |= speed << 3;     /* ����SPI�ٶ� */
    __HAL_SPI_ENABLE(&handle_spi1);              /* ʹ��SPI */
}