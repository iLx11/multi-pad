/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/25 13:45
* @version: 1.0
* @description: 
********************************************************************************/


#ifndef MULTI_OLED_42_H
#define MULTI_OLED_42_H

#include "stm32f1xx_hal.h"

#define SIZE_42    360
#define SIZE_96    999


// 定义软件 SPI
#define SOFT_42_SPI 0

#define OLED_42_NUM_ 2

#define PAGE_42_NUM 5
#define PAGE_42_WIDTH 88

#define OLED_42_WIDTH 70
#define OLED_42_HEIGHT 40

#if SOFT_42_SPI

#define OLED_42_SPI_SOFT_SCL_GPIO GPIOB
#define OLED_42_SPI_SOFT_SDA_GPIO GPIOB
#define OLED_42_SPI_SOFT_SCL_PIN GPIO_PIN_3
#define OLED_42_SPI_SOFT_SDA_PIN GPIO_PIN_5

#define ENABLE_OLED_42_SPI_RCC __HAL_RCC_GPIOB_CLK_ENABLE()

#define OLED_42_SPI_SOFT_SCL_RESET HAL_GPIO_WritePin(OLED_42_SPI_SOFT_SCL_GPIO, OLED_42_SPI_SOFT_SCL_PIN, GPIO_PIN_RESET)
#define OLED_42_SPI_SOFT_SCL_SET HAL_GPIO_WritePin(OLED_42_SPI_SOFT_SCL_GPIO, OLED_42_SPI_SOFT_SCL_PIN, GPIO_PIN_SET)

#define OLED_42_SPI_SOFT_SDA_RESET HAL_GPIO_WritePin(OLED_42_SPI_SOFT_SDA_GPIO, OLED_42_SPI_SOFT_SDA_PIN, GPIO_PIN_RESET)
#define OLED_42_SPI_SOFT_SDA_SET HAL_GPIO_WritePin(OLED_42_SPI_SOFT_SDA_GPIO, OLED_42_SPI_SOFT_SDA_PIN, GPIO_PIN_SET)

#endif

// RES
#define OLED_42_RES_GPIO GPIOD
#define OLED_42_RES_PIN GPIO_PIN_7

// DC
#define OLED_42_DC_GPIO GPIOD
#define OLED_42_DC_PIN GPIO_PIN_6

// CS
#define OLED_42_CS1_GPIO GPIOD
#define OLED_42_CS1_PIN GPIO_PIN_5

#define OLED_42_CS2_GPIO GPIOD
#define OLED_42_CS2_PIN GPIO_PIN_4

#define ENABLE_OLED_42_RCC __HAL_RCC_GPIOD_CLK_ENABLE()

// RES
// 接低电平复位
#define OLED_42_RES_RESET HAL_GPIO_WritePin(OLED_42_RES_GPIO, OLED_42_RES_PIN, GPIO_PIN_RESET)
#define OLED_42_RES_SET HAL_GPIO_WritePin(OLED_42_RES_GPIO, OLED_42_RES_PIN, GPIO_PIN_SET)

// DC
// 命令 -> 拉低 / 数据 -> 拉高
#define OLED_42_DC_RESET  HAL_GPIO_WritePin(OLED_42_DC_GPIO, OLED_42_DC_PIN, GPIO_PIN_RESET)
#define OLED_42_DC_SET  HAL_GPIO_WritePin(OLED_42_DC_GPIO, OLED_42_DC_PIN, GPIO_PIN_SET)

// CS
#define ENABLE_OLED_42_CS(GPIO, PIN) HAL_GPIO_WritePin(GPIO, PIN, GPIO_PIN_RESET)
#define DISABLE_OLED_42_CS(GPIO, PIN) HAL_GPIO_WritePin(GPIO, PIN, GPIO_PIN_SET)

// 写命令
#define OLED_42_CMD  0
// 写数据
#define OLED_42_DATA 1



void oled_42_init(void);

void enable_oled_42_display(uint8_t index);

void disable_oled_42_display(uint8_t index);

void oled_42_refresh_index(uint8_t index);

void oled_42_clear_index(uint8_t index);

void oled_42_show_pic_index(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, uint8_t BMP[], uint8_t mode, uint8_t index);

void oled_42_wt_byte(uint8_t data, uint8_t cmd, uint8_t index);

void oled_42_wt_byte_all(uint8_t data, uint8_t cmd);

void oled_42_page_draw_point(uint8_t x, uint8_t y, uint8_t f);

void oled_42_show_num(uint8_t num, uint8_t mode, uint8_t index);


typedef struct {
    GPIO_TypeDef* gpio_port;
    uint16_t gpio_pin;
} spi_device_cs_42;

// SPI 片选引脚数组
static spi_device_cs_42 spi_device_cs_array_42[2] = {
        {OLED_42_CS1_GPIO, OLED_42_CS1_PIN},
        {OLED_42_CS2_GPIO, OLED_42_CS2_PIN}
};

#endif //MULTI_OLED_42_H
