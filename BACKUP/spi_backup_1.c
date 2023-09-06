
#include "spi111.h"

/* SPI��� */
SPI_HandleTypeDef g_spi2_handle = {0};

/**
 * @brief   ��ʼ��SPI2
 * @param   ��
 * @retval  ��
 */
void spi2_init(void)
{
    /* ����SPI2 */
    g_spi2_handle.Instance = SPI2_SPI;
    g_spi2_handle.Init.Mode = SPI_MODE_MASTER;
    g_spi2_handle.Init.Direction = SPI_DIRECTION_2LINES;
    g_spi2_handle.Init.DataSize = SPI_DATASIZE_8BIT;
    g_spi2_handle.Init.CLKPolarity = SPI_POLARITY_HIGH;
    g_spi2_handle.Init.CLKPhase = SPI_PHASE_2EDGE;
    g_spi2_handle.Init.NSS = SPI_NSS_SOFT;
    g_spi2_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    g_spi2_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    g_spi2_handle.Init.TIMode = SPI_TIMODE_DISABLE;
    g_spi2_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    g_spi2_handle.Init.CRCPolynomial = 1;
    HAL_SPI_Init(&g_spi2_handle);
}

/**
 * @brief   HAL��SPI2��ʼ��MSP����
 * @param   ��
 * @retval  ��
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    if (hspi->Instance == SPI2_SPI)
    {
        /* ʱ��ʹ�� */
        SPI2_SPI_CLK_ENABLE();
        SPI2_SCK_GPIO_CLK_ENABLE();
        SPI2_MISO_GPIO_CLK_ENABLE();
        SPI2_MOSI_GPIO_CLK_ENABLE();
        
        /* ����SCK���� */
        gpio_init_struct.Pin = SPI2_SCK_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(SPI2_SCK_GPIO_PORT, &gpio_init_struct);
        
        /* ����MISO���� */
        gpio_init_struct.Pin = SPI2_MISO_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(SPI2_MISO_GPIO_PORT, &gpio_init_struct);
        
        /* ����MOSI���� */
        gpio_init_struct.Pin = SPI2_MOSI_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(SPI2_MOSI_GPIO_PORT, &gpio_init_struct);
    }
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
void spi2_set_speed(uint32_t speed)
{
    __HAL_SPI_DISABLE(&g_spi2_handle);
    g_spi2_handle.Instance->CR1 &= ~SPI_CR1_BR_Msk;
    g_spi2_handle.Instance->CR1 |= speed;
    __HAL_SPI_ENABLE(&g_spi2_handle);
}
/**
 * @brief   SPI2��дһ�ֽ�����
 * @param   txdata: ��д���һ�ֽ�����
 * @retval  ��ȡ����һ�ֽ�����
 */
uint8_t spi2_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;
    
    if (HAL_SPI_TransmitReceive(&g_spi2_handle, &txdata, &rxdata, 1, 1000) != HAL_OK)
    {
        return 0;
    }
    
    return rxdata;
}
