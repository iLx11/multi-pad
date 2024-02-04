#include "oled.h"
#include <stdio.h>


u8 OLED_GRAM[88][5];

void OLED_42_CS_ALL_OFF() {
    for (int i = 0; i < OLED_NUM; i++) {
        GPIO_TypeDef *port = oled_42_cs_pins[i].port; // 获取端口信息
        uint16_t pin = oled_42_cs_pins[i].pin; // 获取引脚信息
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
    }
}

void OLED_42_CS_SINGLE(u8 index, u8 mode) {

    GPIO_TypeDef *port = oled_42_cs_pins[index].port; // 获取端口信息
    uint16_t pin = oled_42_cs_pins[index].pin; // 获取引脚信息
    if (mode == 0) {
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
    }
    if (mode == 1) {
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
    }

}

void OLED_42_WR_Byte_Single(u8 dat, u8 cmd, u8 index) {
    u8 i;
    if (cmd)
        OLED_42_DC_Set();
    else
        OLED_42_DC_Clr();
    OLED_42_CS_SINGLE(index, 0);
//	for(i=0;i<8;i++)
//	{
//		OLED_SCL_Clr();
//		if(dat&0x80)
//		   OLED_SDA_Set();
//		else
//		   OLED_SDA_Clr();
//		OLED_SCL_Set();
//		dat<<=1;
//	}

    spi2_read_write_byte(dat);
    OLED_42_CS_SINGLE(index, 1);
    //OLED_42_CS_Set();
    OLED_42_DC_Set();
}

void OLED_42_WR_Byte_ALL(u8 dat, u8 cmd) {

    for (int i = 0; i < OLED_NUM; i++) {
        OLED_42_WR_Byte_Single(dat, cmd, i);
    }
}


void OLED_42_Refresh_Single(u8 index) {
    u8 i, n;
    for (i = 0; i < 5; i++) {
        OLED_42_WR_Byte_Single(0xb0 + i, OLED_CMD, index); //设置行起始地址
        OLED_42_WR_Byte_Single(0x0c, OLED_CMD, index);   //设置低列起始地址
        OLED_42_WR_Byte_Single(0x11, OLED_CMD, index);   //设置高列起始地址
        for (n = 0; n < 88; n++)
            OLED_42_WR_Byte_Single(OLED_GRAM[n][i], OLED_DATA, index);
    }
}

void empty_background(uint8_t index, uint8_t mode) {
    if (mode == 0) {
        OLED_42_WR_Byte_Single(0xA4, OLED_CMD, index);
    } else if (mode == 1) {
        OLED_42_WR_Byte_Single(0xA5, OLED_CMD, index);
    }
}


//更新显存到OLED
void OLED_42_Refresh_ALL(void) {
    for (int i = 0; i < OLED_NUM; i++) {
        OLED_42_Refresh_Single(i);
    }
}

void OLED_42_Clear_SINGLE(u8 index) {
    u8 i, n;
    for (i = 0; i < 5; i++) {
        for (n = 0; n < 88; n++) {
            OLED_GRAM[n][i] = 0;//清除所有数据
        }
    }
    OLED_42_Refresh_Single(index);//更新显示
}


//清屏函数
void OLED_42_Clear(void) {
    u8 i, n;
    for (i = 0; i < 5; i++) {
        for (n = 0; n < 88; n++) {
            OLED_GRAM[n][i] = 0;//清除所有数据
        }
    }
    //OLED_42_Refresh_ALL();//更新显示
}
//清屏函数


//开启OLED显示
void OLED_DisPlay_On(void) {
    OLED_42_WR_Byte_ALL(0x8D, OLED_CMD);//电荷泵使能
    OLED_42_WR_Byte_ALL(0x14, OLED_CMD);//开启电荷泵
    OLED_42_WR_Byte_ALL(0xAF, OLED_CMD);//点亮屏幕
}

//关闭OLED显示
void OLED_DisPlay_Off(void) {
    OLED_42_WR_Byte_ALL(0x8D, OLED_CMD);//电荷泵使能
    OLED_42_WR_Byte_ALL(0x10, OLED_CMD);//关闭电荷泵
    OLED_42_WR_Byte_ALL(0xAE, OLED_CMD);//关闭屏幕
}


//画点
//x:0~127
//y:0~63
//t:1 填充 0,清空
void OLED_42_DrawPoint(u8 x, u8 y, u8 t) {
    u8 i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    if (t) {
        OLED_GRAM[x][i] |= n;
    } else {
        OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
        OLED_GRAM[x][i] |= n;
        OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
    }
}


//x,y：起点坐标
//sizex,sizey,图片长宽
//BMP[]：要写入的图片数组
//mode:0,反色显示;1,正常显示
void OLED_42_ShowPicture(u8 x, u8 y, u8 sizex, u8 sizey, u8 BMP[], u8 mode, u8 index) {
    u16 j = 0;
    u8 i, n, temp, m;
    u8 x0 = x, y0 = y;
    sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
    for (n = 0; n < sizey; n++) {
        for (i = 0; i < sizex; i++) {
            temp = BMP[j];
            j++;
            for (m = 0; m < 8; m++) {
                if (temp & 0x01)OLED_42_DrawPoint(x, y, mode);
                else OLED_42_DrawPoint(x, y, !mode);
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
    OLED_42_Refresh_Single(index);
    // OLED_42_Clear();

}


//OLED的初始化
void OLED_42_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;


    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStruct.Pin = OLED_42_SPI_DC_PIN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(OLED_42_SPI_DC_GPIO, &GPIO_InitStruct);


    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStruct.Pin = OLED_42_SPI_RES_PIN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(OLED_42_SPI_RES_GPIO, &GPIO_InitStruct);

    /***********CS1************/
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitStruct.Pin = OLED_42_SPI_CS0_PIN | OLED_42_SPI_CS1_PIN | OLED_42_SPI_CS2_PIN | OLED_42_SPI_CS3_PIN |
                          OLED_42_SPI_CS4_PIN | OLED_42_SPI_CS5_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

    HAL_GPIO_Init(OLED_42_SPI_CS_GROUP_1_GPIO, &GPIO_InitStruct);
    //HAL_GPIO_WritePin(OLED_42_SPI_CS_GROUP_1_GPIO, OLED_42_SPI_CS1_PIN, GPIO_PIN_RESET);
    /***********CS1************/
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitStruct.Pin = OLED_42_SPI_CS6_PIN | OLED_42_SPI_CS7_PIN | OLED_42_SPI_CS8_PIN | OLED_42_SPI_CS9_PIN |
                          OLED_42_SPI_CS10_PIN | OLED_42_SPI_CS11_PIN;
    HAL_GPIO_Init(OLED_42_SPI_CS_GROUP_1_GPIO, &GPIO_InitStruct);
    /***********CS1************/
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitStruct.Pin = OLED_42_SPI_CS12_PIN | OLED_42_SPI_CS13_PIN | OLED_42_SPI_CS14_PIN | OLED_42_SPI_CS15_PIN;
    HAL_GPIO_Init(OLED_42_SPI_CS_GROUP_1_GPIO, &GPIO_InitStruct);
    /***********CS2************/
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStruct.Pin = OLED_42_SPI_CS16_PIN | OLED_42_SPI_CS17_PIN | OLED_42_SPI_CS18_PIN | OLED_42_SPI_CS19_PIN;
    HAL_GPIO_Init(OLED_42_SPI_CS_GROUP_2_GPIO, &GPIO_InitStruct);

    SPI2_Init();

    OLED_42_RES_Clr();
    HAL_Delay(200);
    OLED_42_RES_Set();

    OLED_42_WR_Byte_ALL(0xAE, OLED_CMD); /*display off*/
    OLED_42_WR_Byte_ALL(0xD5, OLED_CMD); /*set osc division*/
    OLED_42_WR_Byte_ALL(0xF0, OLED_CMD);
    OLED_42_WR_Byte_ALL(0xA8, OLED_CMD); /*multiplex ratio*/
    OLED_42_WR_Byte_ALL(0x27, OLED_CMD); /*duty = 1/40*/
    OLED_42_WR_Byte_ALL(0xD3, OLED_CMD); /*set display offset*/
    OLED_42_WR_Byte_ALL(0x00, OLED_CMD);
    OLED_42_WR_Byte_ALL(0x40, OLED_CMD); /*Set Display Start Line */
    OLED_42_WR_Byte_ALL(0x8d, OLED_CMD); /*set charge pump enable*/
    OLED_42_WR_Byte_ALL(0x14, OLED_CMD);
    OLED_42_WR_Byte_ALL(0x20, OLED_CMD); /*Set page address mode*/
    OLED_42_WR_Byte_ALL(0x02, OLED_CMD);
    OLED_42_WR_Byte_ALL(0xA1, OLED_CMD); /*set segment remap*/
    OLED_42_WR_Byte_ALL(0xC8, OLED_CMD); /*Com scan direction*/
    OLED_42_WR_Byte_ALL(0xDA, OLED_CMD); /*set COM pins*/
    OLED_42_WR_Byte_ALL(0x12, OLED_CMD);
    OLED_42_WR_Byte_ALL(0xAD, OLED_CMD); /*Internal IREF Setting*/
    OLED_42_WR_Byte_ALL(0x30, OLED_CMD);
    OLED_42_WR_Byte_ALL(0x81, OLED_CMD); /*contract control*/
    OLED_42_WR_Byte_ALL(0xfF, OLED_CMD); /*128*/
    OLED_42_WR_Byte_ALL(0xD9, OLED_CMD); /*set pre-charge period*/
    OLED_42_WR_Byte_ALL(0x22, OLED_CMD);
    OLED_42_WR_Byte_ALL(0xdb, OLED_CMD); /*set vcomh*/
    OLED_42_WR_Byte_ALL(0x20, OLED_CMD);
    OLED_42_WR_Byte_ALL(0xA4, OLED_CMD); /*Set Entire Display On/Off*/
    OLED_42_WR_Byte_ALL(0xA6, OLED_CMD); /*normal / reverse*/
    OLED_42_WR_Byte_ALL(0x0C, OLED_CMD); /*set lower column address*/
    OLED_42_WR_Byte_ALL(0x11, OLED_CMD); /*set higher column address*/
    //  OLED_42_Clear();
    OLED_42_WR_Byte_ALL(0xAF, OLED_CMD); /*display ON*/
    //  OLED_42_CS_ALL_OFF();
}