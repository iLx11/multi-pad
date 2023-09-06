#ifndef __OLED_H
#define __OLED_H

#include "stdlib.h"
#include "stm32f1xx_hal.h"
#include "system_stm32f1xx.h"
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

#define OLED_NUM   20

//DC
#define OLED_42_SPI_DC_GPIO    GPIOD
#define OLED_42_SPI_DC_PIN    GPIO_PIN_10

//RES
#define OLED_42_SPI_RES_GPIO              GPIOD
#define OLED_42_SPI_RES_PIN                GPIO_PIN_11


//RES
#define OLED_42_RES_Clr() HAL_GPIO_WritePin(OLED_42_SPI_RES_GPIO,OLED_42_SPI_RES_PIN, GPIO_PIN_RESET)//接低电平复位
#define OLED_42_RES_Set() HAL_GPIO_WritePin(OLED_42_SPI_RES_GPIO,OLED_42_SPI_RES_PIN, GPIO_PIN_SET)
//DC
#define OLED_42_DC_Clr()  HAL_GPIO_WritePin(OLED_42_SPI_DC_GPIO,OLED_42_SPI_DC_PIN, GPIO_PIN_RESET)//命令 拉低
#define OLED_42_DC_Set()  HAL_GPIO_WritePin(OLED_42_SPI_DC_GPIO,OLED_42_SPI_DC_PIN, GPIO_PIN_SET)//数据

/***************************CS******************************/
//CS_GROUP_1
#define OLED_42_SPI_CS_GROUP_1_GPIO     GPIOE

//CS0
#define OLED_42_SPI_CS0_PIN    GPIO_PIN_0
//CS1
#define OLED_42_SPI_CS1_PIN    GPIO_PIN_1
//CS2
#define OLED_42_SPI_CS2_PIN    GPIO_PIN_2
//CS3
#define OLED_42_SPI_CS3_PIN    GPIO_PIN_3
//CS4
#define OLED_42_SPI_CS4_PIN    GPIO_PIN_4
//CS5
#define OLED_42_SPI_CS5_PIN    GPIO_PIN_5
//CS6
#define OLED_42_SPI_CS6_PIN    GPIO_PIN_6
//CS7
#define OLED_42_SPI_CS7_PIN    GPIO_PIN_7
//CS8
#define OLED_42_SPI_CS8_PIN    GPIO_PIN_8
//CS9
#define OLED_42_SPI_CS9_PIN    GPIO_PIN_9
//CS10
#define OLED_42_SPI_CS10_PIN    GPIO_PIN_10
//CS11
#define OLED_42_SPI_CS11_PIN    GPIO_PIN_11
//CS12
#define OLED_42_SPI_CS12_PIN    GPIO_PIN_12
//CS13
#define OLED_42_SPI_CS13_PIN    GPIO_PIN_13
//CS14
#define OLED_42_SPI_CS14_PIN    GPIO_PIN_14
//CS15
#define OLED_42_SPI_CS15_PIN    GPIO_PIN_15

//CS_GROUP_2
#define OLED_42_SPI_CS_GROUP_2_GPIO     GPIOD

//CS16
#define OLED_42_SPI_CS16_PIN    GPIO_PIN_12
//CS17
#define OLED_42_SPI_CS17_PIN    GPIO_PIN_13
//CS18
#define OLED_42_SPI_CS18_PIN    GPIO_PIN_14
//CS19
#define OLED_42_SPI_CS19_PIN    GPIO_PIN_15


#define OLED_42_CS_0              OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS0_PIN
#define OLED_42_CS_1              OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS1_PIN
#define OLED_42_CS_2              OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS2_PIN
#define OLED_42_CS_3              OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS3_PIN
#define OLED_42_CS_4              OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS4_PIN
#define OLED_42_CS_5              OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS5_PIN
#define OLED_42_CS_6              OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS6_PIN
#define OLED_42_CS_7              OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS7_PIN
#define OLED_42_CS_8              OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS8_PIN
#define OLED_42_CS_9              OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS9_PIN
#define OLED_42_CS_10            OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS10_PIN
#define OLED_42_CS_11            OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS11_PIN
#define OLED_42_CS_12            OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS12_PIN
#define OLED_42_CS_13            OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS13_PIN
#define OLED_42_CS_14            OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS14_PIN
#define OLED_42_CS_15            OLED_42_SPI_CS_GROUP_1_GPIO,OLED_42_SPI_CS15_PIN
#define OLED_42_CS_16            OLED_42_SPI_CS_GROUP_2_GPIO,OLED_42_SPI_CS16_PIN
#define OLED_42_CS_17            OLED_42_SPI_CS_GROUP_2_GPIO,OLED_42_SPI_CS17_PIN
#define OLED_42_CS_18            OLED_42_SPI_CS_GROUP_2_GPIO,OLED_42_SPI_CS18_PIN
#define OLED_42_CS_19            OLED_42_SPI_CS_GROUP_2_GPIO,OLED_42_SPI_CS19_PIN


typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} CS_42_PinInfo;

static CS_42_PinInfo oled_42_cs_pins[OLED_NUM] =
        {
                {OLED_42_CS_0}, //OLED_CS0
                {OLED_42_CS_1}, //OLED_CS1
                {OLED_42_CS_2}, //OLED_CS2
                {OLED_42_CS_3}, //OLED_CS3
                {OLED_42_CS_4}, //OLED_CS4
                {OLED_42_CS_5}, //OLED_CS5
                {OLED_42_CS_6}, //OLED_CS6
                {OLED_42_CS_7}, //OLED_CS7
                {OLED_42_CS_8}, //OLED_CS8
                {OLED_42_CS_9}, //OLED_CS9
                {OLED_42_CS_10}, //OLED_CS10
                {OLED_42_CS_11}, //OLED_CS11
                {OLED_42_CS_12}, //OLED_CS12
                {OLED_42_CS_13}, //OLED_CS13
                {OLED_42_CS_14}, //OLED_CS14
                {OLED_42_CS_15}, //OLED_CS15
                {OLED_42_CS_16}, //OLED_CS16
                {OLED_42_CS_17}, //OLED_CS17
                {OLED_42_CS_18}, //OLED_CS18
                {OLED_42_CS_19}, //OLED_CS19
        };


#define OLED_CMD  0  //写命令
#define OLED_DATA 1  //写数据

void OLED_42_ClearPoint(u8 x, u8 y);

void OLED_42_ColorTurn(u8 i);

void OLED_42_DisplayTurn(u8 i);

void OLED_42_WR_Byte(u8 dat, u8 mode);

void OLED_42_Refresh_ALL(void);

void OLED_42_Refresh_Single(u8 index);

void OLED_42_Clear(void);

void OLED_42_DrawPoint(u8 x, u8 y, u8 t);

void OLED_42_ShowPicture(u8 x, u8 y, u8 sizex, u8 sizey, u8 BMP[], u8 mode, u8 index);

void OLED_42_Init(void);

#endif

