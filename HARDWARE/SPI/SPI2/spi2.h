#ifndef LEDEN_SPI2_H
#define LEDEN_SPI2_H

#include "stm32f1xx_hal.h"

/*************** pin config *****************/
#define OLED_42_SPI				SPI2
#define OLED_42_SPI_GPIO		GPIOB
#define OLED_42_SPI_CLK_PIN			GPIO_PIN_13
#define OLED_42_SPI_MOSI_PIN		GPIO_PIN_15

void SPI2_Init(void);
uint8_t spi2_read_write_byte(uint8_t txdata);
#endif
