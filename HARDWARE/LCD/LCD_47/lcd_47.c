/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/23 20:42
* @version: 1.0
* @description: 
********************************************************************************/


#include "lcd_47.h"

#include <stdio.h>

#include "spi2.h"
#include "lcdfont.h"


/********************************************************************************
* LCD 初始化
********************************************************************************/
void lcd_47_init(void) {

    lcd_gpio_init();
    LCD_47_RES_RESET;
    HAL_Delay(200);
    LCD_47_RES_SET;

    lcd_wt_byte_all(0x11, LCD_CMD);
    lcd_wt_byte_all(0x36, LCD_CMD);

    // 0x00, 0xC0, 0x70, 0xA0
    if (LCD_DIRECTION == 0) lcd_wt_byte_all(0x70, LCD_DATA);
    else if (LCD_DIRECTION == 1) lcd_wt_byte_all(0x00, LCD_CMD);

    lcd_wt_byte_all(0x3A, LCD_CMD);
    lcd_wt_byte_all(0x05, LCD_DATA);

    lcd_wt_byte_all(0xB2, LCD_CMD);
    lcd_wt_byte_all(0x0C, LCD_DATA);
    lcd_wt_byte_all(0x0C, LCD_DATA);
    lcd_wt_byte_all(0x00, LCD_DATA);
    lcd_wt_byte_all(0x33, LCD_DATA);
    lcd_wt_byte_all(0x33, LCD_DATA);

    lcd_wt_byte_all(0xB7, LCD_CMD);
    lcd_wt_byte_all(0x35, LCD_DATA);

    lcd_wt_byte_all(0xBB, LCD_CMD);
    lcd_wt_byte_all(0x35, LCD_DATA);

    lcd_wt_byte_all(0xC0, LCD_CMD);
    lcd_wt_byte_all(0x2C, LCD_DATA);

    lcd_wt_byte_all(0xC2, LCD_CMD);
    lcd_wt_byte_all(0x01, LCD_DATA);

    lcd_wt_byte_all(0xC3, LCD_CMD);
    lcd_wt_byte_all(0x13, LCD_DATA);

    lcd_wt_byte_all(0xC4, LCD_CMD);
    lcd_wt_byte_all(0x20, LCD_DATA);

    lcd_wt_byte_all(0xC6, LCD_CMD);
    lcd_wt_byte_all(0x0F, LCD_DATA);

    lcd_wt_byte_all(0xD0, LCD_CMD);
    lcd_wt_byte_all(0xA4, LCD_DATA);
    lcd_wt_byte_all(0xA1, LCD_DATA);

    lcd_wt_byte_all(0xD6, LCD_CMD);
    lcd_wt_byte_all(0xA1, LCD_DATA);

    lcd_wt_byte_all(0xE0, LCD_CMD);
    lcd_wt_byte_all(0xF0, LCD_DATA);
    lcd_wt_byte_all(0x00, LCD_DATA);
    lcd_wt_byte_all(0x04, LCD_DATA);
    lcd_wt_byte_all(0x04, LCD_DATA);
    lcd_wt_byte_all(0x04, LCD_DATA);
    lcd_wt_byte_all(0x05, LCD_DATA);
    lcd_wt_byte_all(0x29, LCD_DATA);
    lcd_wt_byte_all(0x33, LCD_DATA);
    lcd_wt_byte_all(0x3E, LCD_DATA);
    lcd_wt_byte_all(0x38, LCD_DATA);
    lcd_wt_byte_all(0x12, LCD_DATA);
    lcd_wt_byte_all(0x12, LCD_DATA);
    lcd_wt_byte_all(0x28, LCD_DATA);
    lcd_wt_byte_all(0x30, LCD_DATA);

    lcd_wt_byte_all(0xE1, LCD_CMD);
    lcd_wt_byte_all(0xF0, LCD_DATA);
    lcd_wt_byte_all(0x07, LCD_DATA);
    lcd_wt_byte_all(0x0A, LCD_DATA);
    lcd_wt_byte_all(0x0D, LCD_DATA);
    lcd_wt_byte_all(0x0B, LCD_DATA);
    lcd_wt_byte_all(0x07, LCD_DATA);
    lcd_wt_byte_all(0x28, LCD_DATA);
    lcd_wt_byte_all(0x33, LCD_DATA);
    lcd_wt_byte_all(0x3E, LCD_DATA);
    lcd_wt_byte_all(0x36, LCD_DATA);
    lcd_wt_byte_all(0x14, LCD_DATA);
    lcd_wt_byte_all(0x14, LCD_DATA);
    lcd_wt_byte_all(0x29, LCD_DATA);
    lcd_wt_byte_all(0x32, LCD_DATA);

// 	lcd_wt_byte_all(0x2A,LCD_CMD);
//	lcd_wt_byte_all(0x00, LCD_DATA);
//	lcd_wt_byte_all(0x22, LCD_DATA);
//	lcd_wt_byte_all(0x00, LCD_DATA);
//	lcd_wt_byte_all(0xCD, LCD_DATA);
//	lcd_wt_byte_all(0x2B, LCD_DATA);
//	lcd_wt_byte_all(0x00, LCD_DATA);
//	lcd_wt_byte_all(0x00, LCD_DATA);
//	lcd_wt_byte_all(0x01, LCD_DATA);
//	lcd_wt_byte_all(0x3F, LCD_DATA);
//	lcd_wt_byte_all(0x2C,LCD_CMD);
    lcd_wt_byte_all(0x21, LCD_CMD);

    lcd_wt_byte_all(0x11, LCD_CMD);
    HAL_Delay(120);
    lcd_wt_byte_all(0x29, LCD_CMD);
}

/********************************************************************************
* LCD GPIO 初始化
********************************************************************************/
static void lcd_gpio_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    ENABLE_LCD_47_RCC;
    GPIO_InitStruct.Pin = LCD_47_RES_PIN | LCD_47_DC_PIN | LCD_47_CS1_PIN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;

    HAL_GPIO_Init(LCD_47_RES_GPIO, &GPIO_InitStruct);

    __HAL_RCC_GPIOA_CLK_ENABLE();
    // BLK
    GPIO_InitStruct.Pin = LCD_47_BLK_PIN;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(LCD_47_BLK_GPIO, &GPIO_InitStruct);
//    HAL_Delay(2000);
    HAL_GPIO_WritePin(LCD_47_BLK_GPIO, LCD_47_BLK_PIN, GPIO_PIN_RESET);


//    HAL_GPIO_WritePin(LCD_47_RES_GPIO, LCD_47_RES_PIN | LCD_47_DC_PIN | LCD_47_CS1_PIN, GPIO_PIN_SET);

#if SOFT_SPI
    __HAL_RCC_GPIOB_CLK_ENABLE();

    //GPIO初始化设置
    GPIO_InitStruct.Pin = LCD_SPI_SOFT_SCL_PIN | LCD_SPI_SOFT_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULLUP;//上拉
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);//初始化
//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10 | GPIO_PIN_15, GPIO_PIN_SET);
#else
    // spi2 初始化
    LCD_SPI_INIT_FUNC();
#endif
}


/********************************************************************************
* 发送数据到所有设备
********************************************************************************/
void lcd_wt_byte_all(uint8_t data, uint8_t cmd) {
    for (int i = 0; i < LCD_47_NUM; i++) {
        lcd_wt_byte(data, cmd, i);
    }
}

/********************************************************************************
* 传输单字节数据
********************************************************************************/
void lcd_wt_byte(uint8_t data, uint8_t cmd, uint8_t index) {
    if (cmd)
        LCD_47_DC_SET;
    else
        LCD_47_DC_RESET;
    // 使能片选,有多个从设备时,需要根据不同的片选线使能
    ENABLE_LCD_47_CS(spi_device_cs_array[index].gpio_port, spi_device_cs_array[index].gpio_pin);
#if SOFT_SPI
    lcd_send_byte(data);
#else
    // 硬件 SPI
    // 发送数据
//    spi2_transmit_receive_byte(data);
    LCD_SPI_TRANS_BYTE_FUNC(data);
#endif
    // 失能片选
    DISABLE_LCD_47_CS(spi_device_cs_array[index].gpio_port, spi_device_cs_array[index].gpio_pin);
    LCD_47_DC_SET;
}

/********************************************************************************
* 传输双字节数据
********************************************************************************/
static void lcd_wt_d_byte(uint16_t data, uint8_t index) {
    lcd_wt_byte(data >> 8, LCD_DATA, index);
    lcd_wt_byte(data, LCD_DATA, index);
}

#if SOFT_SPI

/********************************************************************************
* 软件 SPI 传输单字节
********************************************************************************/
static void lcd_send_byte(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        // 拉低时钟
        LCD_SPI_SOFT_SCL_RESET;
        // 判断数据
        if (data & 0x80)
            LCD_SPI_SOFT_SDA_SET;
        else
            LCD_SPI_SOFT_SDA_RESET;
        // 拉高时钟
        LCD_SPI_SOFT_SCL_SET;
        // 数据移到下一位
        data <<= 1;
    }
}

#else

void transmit_image_data(uint8_t* pic, uint32_t len) {
    uint32_t offset = 0;
    // 每次传输 4096 字节
    uint32_t chunk_size = 4096;
    while (offset < len) {
        // 计算每次传输的数据大小
        uint16_t size = chunk_size < (len - offset) ? chunk_size : (len - offset);

        LCD_SPI_TRANS_FUNC(pic + offset, size);
        // 更新偏移
        offset += size;
    }
}

#endif

/********************************************************************************
* 设置屏幕地址，光标位置
********************************************************************************/
void set_lcd_address(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t index) {
    if (LCD_DIRECTION == 0) {
        lcd_wt_byte(0x2a, LCD_CMD, index);
        lcd_wt_d_byte(x1, index);
        lcd_wt_d_byte(x2, index);
        lcd_wt_byte(0x2b, LCD_CMD, index);
        lcd_wt_d_byte(y1 + 34, index);
        lcd_wt_d_byte(y2 + 34, index);
        lcd_wt_byte(0x2c, LCD_CMD, index);

    } else {
        lcd_wt_byte(0x2A, LCD_CMD, index);
        lcd_wt_d_byte(x1 + 34, index);
        lcd_wt_d_byte(x2 + 34, index);
        lcd_wt_byte(0x2B, LCD_CMD, index);
        lcd_wt_d_byte(y1, index);
        lcd_wt_d_byte(y2, index);
        lcd_wt_byte(0x2C, LCD_CMD, index);
    }
}

/********************************************************************************
* LCD 显示图片
********************************************************************************/
void lcd_show_pic_index(uint8_t x, uint8_t y, uint16_t sizex, uint16_t sizey, uint8_t* pic, uint8_t index) {
    uint16_t i, j;
    uint32_t k = 0;
    set_lcd_address(x, y, x + sizex - 1, y + sizey - 1, index);
#if SOFT_SPI
    for (i = 0; i < sizex; i++) {
        for (j = 0; j < sizey; j++) {
            lcd_wt_byte(pic[k * 2], LCD_DATA, index);
            lcd_wt_byte(pic[k * 2 + 1], LCD_DATA, index);
            k++;
        }
    }
#else
    LCD_47_DC_SET;
    ENABLE_LCD_47_CS(spi_device_cs_array[index].gpio_port, spi_device_cs_array[index].gpio_pin);

    transmit_image_data((uint8_t *)pic, sizex * sizey * 2);

    DISABLE_LCD_47_CS(spi_device_cs_array[index].gpio_port, spi_device_cs_array[index].gpio_pin);

#endif
}

/********************************************************************************
* LCD page 屏幕画点
********************************************************************************/
void lcd_page_draw_point(uint8_t x, uint8_t y, uint16_t color_data, uint8_t index) {
    set_lcd_address(x, y, x, y, index);
    lcd_wt_d_byte(color_data, index);
}


/********************************************************************************
* 填充整个屏幕
********************************************************************************/
void lcd_fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color, uint8_t index) {
    uint16_t i, j;
    set_lcd_address(xsta, ysta, xend - 1, yend - 1, index);//设置显示范围
    for (i = ysta; i < yend; i++) {
        for (j = xsta; j < xend; j++) {
            lcd_wt_d_byte(color, index);
        }
    }
}

/********************************************************************************
* LCD 显示单个字符
********************************************************************************/
void lcd_show_char_index(uint16_t x, uint16_t y, uint8_t num, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode, uint8_t index) {
    uint8_t temp, sizex, t, m = 0;
    // 一个字符所占字节大小
    uint16_t i, TypefaceNum;
    uint16_t x0 = x;
    sizex = sizey / 2;
    TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
    // 得到偏移后的值
    num = num - ' ';
    set_lcd_address(x, y, x + sizex - 1, y + sizey - 1, index);  //设置光标位置
    for (i = 0; i < TypefaceNum; i++) {
        if (sizey == 12) temp = ascii_1206[num][i];               //调用6x12字体
        else if (sizey == 16) temp = ascii_1608[num][i];         //调用8x16字体
        else if (sizey == 24) temp = ascii_2412[num][i];         //调用12x24字体
        else if (sizey == 32) temp = ascii_3216[num][i];         //调用16x32字体
        else return;
        for (t = 0; t < 8; t++) {
            // 非叠加模式
            if (!mode) {
                if (temp & (0x01 << t))
                    lcd_wt_d_byte(fc, index);
                else lcd_wt_d_byte(bc, index);
                m++;
                if (m % sizex == 0) {
                    m = 0;
                    break;
                }
                // 叠加模式
            } else {
                if (temp & (0x01 << t))
                    lcd_page_draw_point(x, y, fc, index);//画一个点
                x++;
                if ((x - x0) == sizex) {
                    x = x0;
                    y++;
                    break;
                }
            }
        }
    }
}

/********************************************************************************
* LCD 显示字符串
* 函数说明：显示单个字符
  入口数据：  x,y显示坐标
            num 要显示的字符
            fc 字的颜色
            bc 字的背景色
            sizey 字号
            mode:  0非叠加模式  1叠加模式
  返回值：  无
********************************************************************************/
void lcd_show_string(uint16_t x, uint16_t y, const uint8_t *p, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode, uint8_t index) {
    while (*p != '\0') {
        lcd_show_char_index(x, y, *p, fc, bc, sizey, mode, index);
        x += sizey / 2;
        p++;
    }
}