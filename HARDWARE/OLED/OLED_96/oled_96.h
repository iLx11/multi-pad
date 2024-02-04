#ifndef LEDEN_OLED_96_H
#define LEDEN_OLED_96_H
#include "stdlib.h"
#include "stm32f1xx_hal.h"
#include "spi3.h"
#include "spi2.h"
#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

//-----------------OLED端口定义----------------
#define OLED_92_NUM 						5
#define OLED_92_SPI_GPIO				GPIOB
#define OLED_92_SPI_SCK_PIN    	GPIO_PIN_3//SCL
#define OLED_92_SPI_MOSI_PIN		GPIO_PIN_5//SDA
#define OLED_92_SPI_DC_PIN		  GPIO_PIN_11//DC
#define OLED_92_SPI_RES_PIN    	GPIO_PIN_10//RES
/**********CS******/
//GROUP_1
#define OLED_92_SPI_CS_GROUP_1_GPIO				GPIOA
#define OLED_92_SPI_CS0_PIN    						GPIO_PIN_1//CS
#define OLED_92_SPI_CS1_PIN    						GPIO_PIN_2//CS
#define OLED_92_SPI_CS2_PIN     					GPIO_PIN_3//CS
//GROUP_2
#define OLED_92_SPI_CS_GROUP_2_GPIO				GPIOB
#define OLED_92_SPI_CS3_PIN     GPIO_PIN_8//CS
#define OLED_92_SPI_CS4_PIN     GPIO_PIN_9//CS
#define OLED_92_SW_PIN       		GPIO_PIN_0//SW
//SCL
#define OLED_92_SCL_Clr() HAL_GPIO_WritePin(OLED_92_SPI_GPIO,OLED_92_SPI_SCK_PIN, GPIO_PIN_RESET)
#define OLED_92_SCL_Set() HAL_GPIO_WritePin(OLED_92_SPI_GPIO,OLED_92_SPI_SCK_PIN, GPIO_PIN_SET)
//SDA
#define OLED_92_SDA_Clr() HAL_GPIO_WritePin(OLED_92_SPI_GPIO,OLED_92_SPI_MOSI_PIN, GPIO_PIN_RESET)//MOSI主设备输出
#define OLED_92_SDA_Set() HAL_GPIO_WritePin(OLED_92_SPI_GPIO,OLED_92_SPI_MOSI_PIN, GPIO_PIN_SET)
//RES
#define OLED_92_RES_Clr() HAL_GPIO_WritePin(OLED_92_SPI_GPIO,OLED_92_SPI_RES_PIN, GPIO_PIN_RESET)//接低电平复位
#define OLED_92_RES_Set() HAL_GPIO_WritePin(OLED_92_SPI_GPIO,OLED_92_SPI_RES_PIN, GPIO_PIN_SET)
//DC
#define OLED_92_DC_Clr()  HAL_GPIO_WritePin(OLED_92_SPI_GPIO,OLED_92_SPI_DC_PIN, GPIO_PIN_RESET)//命令 拉低
#define OLED_92_DC_Set()  HAL_GPIO_WritePin(OLED_92_SPI_GPIO,OLED_92_SPI_DC_PIN, GPIO_PIN_SET)//数据
////CS
//#define OLED_92_CS_Clr()  HAL_GPIO_WritePin(OLED_92_SPI_GPIO,OLED_92_SPI_CS1_PIN)//
//#define OLED_92_CS_Set()  HAL_GPIO_WritePin(OLED_92_SPI_GPIO,OLED_92_SPI_CS1_PIN)//


#define OLED_92_CS_0 							OLED_92_SPI_CS_GROUP_1_GPIO,OLED_92_SPI_CS0_PIN
#define OLED_92_CS_1 							OLED_92_SPI_CS_GROUP_1_GPIO,OLED_92_SPI_CS1_PIN
#define OLED_92_CS_2 							OLED_92_SPI_CS_GROUP_1_GPIO,OLED_92_SPI_CS2_PIN
#define OLED_92_CS_3 							OLED_92_SPI_CS_GROUP_2_GPIO,OLED_92_SPI_CS3_PIN
#define OLED_92_CS_4 							OLED_92_SPI_CS_GROUP_2_GPIO,OLED_92_SPI_CS4_PIN
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} CS_92_PinInfo;


static CS_92_PinInfo oled_92_cs_pins [OLED_92_NUM]=
        {
                {OLED_92_CS_2}, //OLED_CS0
                {OLED_92_CS_1}, //OLED_CS1
                {OLED_92_CS_0}, //OLED_CS2
                {OLED_92_CS_3}, //OLED_CS3
                {OLED_92_CS_4}, //OLED_CS4
        };


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

void OLED_ClearPoint(u8 x,u8 y);

void OLED_WR_Byte(u8 dat,u8 mode);

void OLED_92_Refresh(void);
void OLED_92_Clear(void);
//void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode, u8 index);

void OLED_92_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode,u8 index);
void OLED_92_Init(void);

#endif //LEDEN_OLED_96_H
