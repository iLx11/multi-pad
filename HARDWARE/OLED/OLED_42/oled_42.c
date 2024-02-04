/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/25 13:45
* @version: 1.0
* @description: 
********************************************************************************/


#include "oled_42.h"
#include "spi3.h"

// 显示数据,128 * 32,4 -> (32 / 8)
uint8_t oled_42_page[PAGE_42_WIDTH][PAGE_42_NUM];

/********************************************************************************
* OLED 初始化
********************************************************************************/
void oled_42_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    ENABLE_OLED_42_RCC;
    GPIO_InitStruct.Pin = OLED_42_RES_PIN | OLED_42_DC_PIN | OLED_42_CS1_PIN | OLED_42_CS2_PIN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(OLED_42_RES_GPIO, &GPIO_InitStruct);
#if SOFT_42_SPI
    ENABLE_OLED_42_SPI_RCC;

    //GPIO初始化设置
    GPIO_InitStruct.Pin = OLED_42_SPI_SOFT_SCL_PIN | OLED_42_SPI_SOFT_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULLUP;//上拉
    HAL_GPIO_Init( OLED_42_SPI_SOFT_SCL_GPIO,&GPIO_InitStruct);//初始化
#else
    // spi3 初始化
    spi3_init();
#endif

    OLED_42_RES_RESET;
    HAL_Delay(200);
    OLED_42_RES_SET;

    oled_42_wt_byte_all(0xAE, OLED_42_CMD); /*display off*/
    oled_42_wt_byte_all(0xD5, OLED_42_CMD); /*set osc division*/
    oled_42_wt_byte_all(0xF0, OLED_42_CMD);
    oled_42_wt_byte_all(0xA8, OLED_42_CMD); /*multiplex ratio*/
    oled_42_wt_byte_all(0x27, OLED_42_CMD); /*duty = 1/40*/
    oled_42_wt_byte_all(0xD3, OLED_42_CMD); /*set display offset*/
    oled_42_wt_byte_all(0x00, OLED_42_CMD);
    oled_42_wt_byte_all(0x40, OLED_42_CMD); /*Set Display Start Line */
    oled_42_wt_byte_all(0x8d, OLED_42_CMD); /*set charge pump enable*/
    oled_42_wt_byte_all(0x14, OLED_42_CMD);
    oled_42_wt_byte_all(0x20, OLED_42_CMD); /*Set page address mode*/
    oled_42_wt_byte_all(0x02, OLED_42_CMD);
    oled_42_wt_byte_all(0xA1, OLED_42_CMD); /*set segment remap*/
    oled_42_wt_byte_all(0xC8, OLED_42_CMD); /*Com scan direction*/
    oled_42_wt_byte_all(0xDA, OLED_42_CMD); /*set COM pins*/
    oled_42_wt_byte_all(0x12, OLED_42_CMD);
    oled_42_wt_byte_all(0xAD, OLED_42_CMD); /*Internal IREF Setting*/
    oled_42_wt_byte_all(0x30, OLED_42_CMD);
    oled_42_wt_byte_all(0x81, OLED_42_CMD); /*contract control*/
    oled_42_wt_byte_all(0xfF, OLED_42_CMD); /*128*/
    oled_42_wt_byte_all(0xD9, OLED_42_CMD); /*set pre-charge period*/
    oled_42_wt_byte_all(0x22, OLED_42_CMD);
    oled_42_wt_byte_all(0xdb, OLED_42_CMD); /*set vcomh*/
    oled_42_wt_byte_all(0x20, OLED_42_CMD);
    oled_42_wt_byte_all(0xA4, OLED_42_CMD); /*Set Entire Display On/Off*/
    oled_42_wt_byte_all(0xA6, OLED_42_CMD); /*normal / reverse*/
    oled_42_wt_byte_all(0x0C, OLED_42_CMD); /*set lower column address*/
    oled_42_wt_byte_all(0x11, OLED_42_CMD); /*set higher column address*/
    for(uint8_t i = 0; i < OLED_42_NUM_; i ++) {
        oled_42_clear_index(i);
    }
    oled_42_wt_byte_all(0xAF, OLED_42_CMD); /*display ON*/
}

/********************************************************************************
* 开启 OLED 显示
********************************************************************************/
void enable_oled_42_display(uint8_t index) {
    // 电荷泵使能
    oled_42_wt_byte_all(0x8D, OLED_42_CMD);
    // 开启总电源
    oled_42_wt_byte_all(0x14, OLED_42_CMD);
    // 点亮屏幕
    oled_42_wt_byte_all(0xAF, OLED_42_CMD);
}

/********************************************************************************
* 关闭 OLED 显示
********************************************************************************/
void disable_oled_42_display(uint8_t index) {
    // 电荷泵使能
    oled_42_wt_byte_all(0x8D, OLED_42_CMD);
    // 关闭总电源
    oled_42_wt_byte_all(0x10, OLED_42_CMD);
    // 关闭屏幕
    oled_42_wt_byte_all(0xAE, OLED_42_CMD);
}


/********************************************************************************
* OLED 根据片选线更新显示
********************************************************************************/
void oled_42_refresh_index(uint8_t index) {
    for(uint8_t i = 0; i < PAGE_42_NUM; i ++) {
        oled_42_wt_byte(0xb0 + i, OLED_42_CMD, index);
        oled_42_wt_byte(0x0C, OLED_42_CMD, index);
        oled_42_wt_byte(0x11, OLED_42_CMD, index);
        for(uint8_t j = 0; j < OLED_42_WIDTH; j ++)
            oled_42_wt_byte(oled_42_page[j][i], OLED_42_DATA, index);
    }
}

/********************************************************************************
* OLED 根据片选线清除显示
********************************************************************************/
void oled_42_clear_index(uint8_t index) {
    for(uint8_t i = 0; i < PAGE_42_NUM; i ++){
        for(uint8_t j = 0; j < OLED_42_WIDTH; j ++){
            oled_42_page[j][i] = 0;
        }
    }
    oled_42_refresh_index(index);
}

/********************************************************************************
* 发送数据到所有设备
********************************************************************************/
void oled_42_wt_byte_all(uint8_t data, uint8_t cmd) {
    for (int i = 0; i < OLED_42_NUM_; i++) {
        oled_42_wt_byte(data, cmd, i);
    }
}

/********************************************************************************
* 传输单字节数据
********************************************************************************/
void oled_42_wt_byte(uint8_t data, uint8_t cmd, uint8_t index) {
    if(cmd)
        OLED_42_DC_SET;
    else
        OLED_42_DC_RESET;
    // 使能片选,有多个从设备时,需要根据不同的片选线使能
    ENABLE_OLED_42_CS(spi_device_cs_array_42[index].gpio_port, spi_device_cs_array_42[index].gpio_pin);
#if SOFT_42_SPI
    for(uint8_t i = 0; i < 8; i ++) {
        // 拉低时钟
        OLED_42_SPI_SOFT_SCL_RESET;
        // 判断数据
        if(data & 0x80)
            OLED_42_SPI_SOFT_SDA_SET;
        else
            OLED_42_SPI_SOFT_SDA_RESET;
        // 拉高时钟
        OLED_42_SPI_SOFT_SCL_SET;
        // 数据移到下一位
        data <<= 1;
    }
#else
    // 硬件 SPI
    // 发送数据
    spi3_transmit_receive_byte(data);
#endif
    // 失能片选
    DISABLE_OLED_42_CS(spi_device_cs_array_42[index].gpio_port, spi_device_cs_array_42[index].gpio_pin);
    OLED_42_DC_SET;
}

/********************************************************************************
* OLED 显示图片
********************************************************************************/
void oled_42_show_pic_index(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, uint8_t BMP[], uint8_t mode, uint8_t index) {
    uint16_t j = 0;
    uint8_t i, n, temp, m;
    uint8_t x0 = x, y0 = y;
    sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
    for (n = 0; n < sizey; n++) {
        for (i = 0; i < sizex; i++) {
            temp = BMP[j];
            j++;
            for (m = 0; m < 8; m++) {
                if (temp & 0x01)
                    oled_42_page_draw_point(x, y, mode);
                else
                    oled_42_page_draw_point(x, y, !mode);
                temp >>= 1;
                y++;
            }
            x++;
            if ((x - x0) == sizex) {
                x = x0;
                y0 = y0 + 8;
            }
            y = y0;
        }
    }
    oled_42_refresh_index(index);
}

/********************************************************************************
* OLED page 数组画点
********************************************************************************/
void oled_42_page_draw_point(uint8_t x, uint8_t y, uint8_t f) {
    uint8_t page = y / 8;
    uint8_t page_byte = y % 8;
    uint8_t set = (1 << page_byte);
    if(f) {
        // 设为 1 填充
        oled_42_page[x][page] |= set;
    } else {
        // 设为 0 清空
        oled_42_page[x][page] &= (~ set);
        // 另一种方式
        /*oled_42_page[x][page] = ~oled_42_page[x][page];
        oled_42_page[x][page] |= set;
        oled_42_page[x][page] = ~oled_42_page[x][page];*/
    }
}
