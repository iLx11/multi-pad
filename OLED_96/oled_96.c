#include "oled_96.h"
//#include "oledfont.h"

u8 OLED_92_GRAM[144][8];

void OLED_92_CS_ALL_OFF() {
    for (int i = 0; i < OLED_92_NUM; i++) {
        GPIO_TypeDef *port = oled_92_cs_pins[i].port; // 获取端口信息
        uint16_t pin = oled_92_cs_pins[i].pin; // 获取引脚信息
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
    }
}

void OLED_92_CS_SINGLE(u8 index, u8 mode) {

    GPIO_TypeDef *port = oled_92_cs_pins[index].port; // 获取端口信息
    uint16_t pin = oled_92_cs_pins[index].pin; // 获取引脚信息
    if (mode == 0) {
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
    }
    if (mode == 1) {
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
    }

}

//void OLED_WR_Byte(u8 dat,u8 cmd)
//{
//    u8 i;
//    if(cmd)
//        OLED_92_DC_Set();
//    else
//        OLED_92_DC_Clr();
//    OLED_92_CS_Clr();
//    for(i=0; i<8; i++)
//    {
//        OLED_92_SCL_Clr();
//        if(dat&0x80)
//            OLED_92_SDA_Set();
//        else
//            OLED_92_SDA_Clr();
//        OLED_92_SCL_Set();
//        dat<<=1;
//    }
//    OLED_92_CS_Set();
//    OLED_92_DC_Set();
//}

void OLED_92_WR_Byte_Single(u8 dat, u8 cmd, u8 index) {
    u8 i;
    if (cmd)
        OLED_92_DC_Set();
    else
        OLED_92_DC_Clr();
    OLED_92_CS_SINGLE(index, 0);
    for (i = 0; i < 8; i++) {
        OLED_92_SCL_Clr();
        if (dat & 0x80)
            OLED_92_SDA_Set();
        else
            OLED_92_SDA_Clr();
        OLED_92_SCL_Set();
        dat <<= 1;
    }


    //SPI0_write_byte(dat);
//    spi2_read_write_byte(dat);
    OLED_92_CS_SINGLE(index, 1);
    //OLED_42_CS_Set();
    OLED_92_DC_Set();
}

void OLED_92_WR_Byte_ALL(u8 dat, u8 cmd) {

    for (int i = 0; i < OLED_92_NUM; i++) {
        OLED_92_WR_Byte_Single(dat, cmd, i);
    }
}

void OLED_92_Refresh_Single(u8 index) {
    u8 i, n;
    for (i = 0; i < 8; i++) {
        OLED_92_WR_Byte_Single(0xb0 + i, OLED_CMD, index); //设置行起始地址
        OLED_92_WR_Byte_Single(0x00, OLED_CMD, index);   //设置低列起始地址
        OLED_92_WR_Byte_Single(0x10, OLED_CMD, index);   //设置高列起始地址
        for (n = 0; n < 128; n++)
            OLED_92_WR_Byte_Single(OLED_92_GRAM[n][i], OLED_DATA, index);
    }
}

void OLED_92_Refresh_ALL(void) {
    for (int i = 0; i < OLED_92_NUM; i++) {
        OLED_92_Refresh_Single(i);
    }
}


//清屏函数
void OLED_92_Clear(void) {
    u8 i, n;
    for (i = 0; i < 8; i++) {
        for (n = 0; n < 128; n++) {
            OLED_92_GRAM[n][i] = 0;//清除所有数据
        }
    }
    // OLED_92_Refresh();//更新显示
}

//画点
//x:0~127
//y:0~63
//t:1 填充 0,清空
void OLED_92_DrawPoint(u8 x, u8 y, u8 t) {
    u8 i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    if (t) {
        OLED_92_GRAM[x][i] |= n;
    } else {
        OLED_92_GRAM[x][i] = ~OLED_92_GRAM[x][i];
        OLED_92_GRAM[x][i] |= n;
        OLED_92_GRAM[x][i] = ~OLED_92_GRAM[x][i];
    }
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size1:选择字体 6x8/6x12/8x16/12x24
//mode:0,反色显示;1,正常显示
/*void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1, u8 mode, u8 index) {
    u8 i, m, temp, size2, chr1;
    u8 x0 = x, y0 = y;
    if (size1 == 8)size2 = 6;
    else size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2);  //得到字体一个字符对应点阵集所占的字节数
    chr1 = chr - ' ';  //计算偏移后的值
    for (i = 0; i < size2; i++) {
        if (size1 == 8) { temp = asc2_0806[chr1][i]; } //调用0806字体
        else if (size1 == 12) { temp = asc2_1206[chr1][i]; } //调用1206字体
        else if (size1 == 16) { temp = asc2_1608[chr1][i]; } //调用1608字体
        else if (size1 == 24) { temp = asc2_2412[chr1][i]; } //调用2412字体
        else return;
        for (m = 0; m < 8; m++) {
            if (temp & 0x01) OLED_92_DrawPoint(x, y, mode);
            else OLED_92_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        if ((size1 != 8) && ((x - x0) == size1 / 2)) {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
    OLED_92_Refresh_Single(index);

    OLED_92_Clear();

}*/


//x,y：起点坐标
//sizex,sizey,图片长宽
//BMP[]：要写入的图片数组
//mode:0,反色显示;1,正常显示
void OLED_92_ShowPicture(u8 x, u8 y, u8 sizex, u8 sizey, u8 BMP[], u8 mode, u8 index) {
    u16 j = 0;
    u8 i, n, temp, m;
    u8 x0 = x, y0 = y;
    sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
    for (n = 0; n < sizey; n++) {
        for (i = 0; i < sizex; i++) {
            temp = BMP[j];
            j++;
            for (m = 0; m < 8; m++) {

                if (temp & 0x01)OLED_92_DrawPoint(x, y, mode);
                else OLED_92_DrawPoint(x, y, !mode);
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
    OLED_92_Refresh_Single(index);
    OLED_92_Clear();
}

//OLED的初始化
void OLED_92_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    __HAL_RCC_AFIO_CLK_ENABLE();  // 使能AFIO外设时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStructure.Pin =
            OLED_92_SPI_SCK_PIN  | OLED_92_SPI_DC_PIN | OLED_92_SPI_MOSI_PIN | OLED_92_SPI_RES_PIN;

//    GPIO_InitStructure.Pin =
//            OLED_92_SPI_DC_PIN  | OLED_92_SPI_RES_PIN;


    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;     //推挽输出
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;//速度50MHz
    HAL_GPIO_Init(OLED_92_SPI_GPIO, &GPIO_InitStructure);    //初始化GPIOG12
    HAL_GPIO_WritePin(OLED_92_SPI_GPIO,
                      OLED_92_SPI_SCK_PIN | OLED_92_SPI_SCK_PIN | OLED_92_SPI_DC_PIN | OLED_92_SPI_MOSI_PIN |
                      OLED_92_SPI_RES_PIN, GPIO_PIN_SET);

//    HAL_GPIO_WritePin(OLED_92_SPI_GPIO,
//                 OLED_92_SPI_DC_PIN |
//                 OLED_92_SPI_RES_PIN, GPIO_PIN_SET);


    /***********CS1************/
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStructure.Pin = OLED_92_SPI_CS0_PIN | OLED_92_SPI_CS1_PIN | OLED_92_SPI_CS2_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(OLED_92_SPI_CS_GROUP_1_GPIO, &GPIO_InitStructure);
    /***********CS2************/
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStructure.Pin = OLED_92_SPI_CS3_PIN | OLED_92_SPI_CS4_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(OLED_92_SPI_CS_GROUP_2_GPIO, &GPIO_InitStructure);
//    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
    __HAL_AFIO_REMAP_SWJ_NOJTAG();

    OLED_92_RES_Clr();
    HAL_Delay(200);
    OLED_92_RES_Set();

    OLED_92_WR_Byte_ALL(0xAE, OLED_CMD);//--turn off oled panel
    OLED_92_WR_Byte_ALL(0x00, OLED_CMD);//---set low column address
    OLED_92_WR_Byte_ALL(0x10, OLED_CMD);//---set high column address
    OLED_92_WR_Byte_ALL(0x40, OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_92_WR_Byte_ALL(0x81, OLED_CMD);//--set contrast control register
    OLED_92_WR_Byte_ALL(0xCF, OLED_CMD);// Set SEG Output Current Brightness
    OLED_92_WR_Byte_ALL(0xA0, OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_92_WR_Byte_ALL(0xC0, OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_92_WR_Byte_ALL(0xA6, OLED_CMD);//--set normal display
    OLED_92_WR_Byte_ALL(0xA8, OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_92_WR_Byte_ALL(0x3f, OLED_CMD);//--1/64 duty
    OLED_92_WR_Byte_ALL(0xD3, OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_92_WR_Byte_ALL(0x00, OLED_CMD);//-not offset
    OLED_92_WR_Byte_ALL(0xd5, OLED_CMD);//--set display clock divide ratio/oscillator frequency
    OLED_92_WR_Byte_ALL(0x80, OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_92_WR_Byte_ALL(0xD9, OLED_CMD);//--set pre-charge period
    OLED_92_WR_Byte_ALL(0xF1, OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_92_WR_Byte_ALL(0xDA, OLED_CMD);//--set com pins hardware configuration
    OLED_92_WR_Byte_ALL(0x12, OLED_CMD);
    OLED_92_WR_Byte_ALL(0xDB, OLED_CMD);//--set vcomh
    OLED_92_WR_Byte_ALL(0x40, OLED_CMD);//Set VCOM Deselect Level
    OLED_92_WR_Byte_ALL(0x20, OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_92_WR_Byte_ALL(0x02, OLED_CMD);//
    OLED_92_WR_Byte_ALL(0x8D, OLED_CMD);//--set Charge Pump enable/disable
    OLED_92_WR_Byte_ALL(0x14, OLED_CMD);//--set(0x10) disable
    OLED_92_WR_Byte_ALL(0xA4, OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
    OLED_92_WR_Byte_ALL(0xA6, OLED_CMD);// Disable Inverse Display On (0xa6/a7)

    OLED_92_Clear();
    OLED_92_WR_Byte_ALL(0xAF, OLED_CMD);
    // OLED_92_CS_ALL_OFF();
}
