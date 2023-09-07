#ifndef __MATRIX_KEYS__
#define __MATRIX_KEYS__

#include "stm32f1xx.h"

//设置行列数
#define  ROW_NUM  4
#define  COL_NUM  5
//GPIO需要的参数
#define ROW_GPIO_RCC RCC_APB2Periph_GPIOA
#define COL_GPIO_RCC RCC_APB2Periph_GPIOB

#define ROW_GPIO     GPIOD
#define COL_GPIO     GPIOD

#define ROW1_PIN    GPIO_PIN_3
#define ROW2_PIN    GPIO_PIN_2
#define ROW3_PIN    GPIO_PIN_1
#define ROW4_PIN    GPIO_PIN_0


#define COL1_PIN    GPIO_PIN_9
#define COL2_PIN    GPIO_PIN_8
#define COL3_PIN    GPIO_PIN_7
#define COL4_PIN    GPIO_PIN_6
#define COL5_PIN    GPIO_PIN_5

//先设置不同行按下的情况，然后扫描列的列为1的情况下就按下。
//读取COL按键电平
#define COL1        HAL_GPIO_ReadPin(COL_GPIO,COL1_PIN)
#define COL2        HAL_GPIO_ReadPin(COL_GPIO,COL2_PIN)
#define COL3        HAL_GPIO_ReadPin(COL_GPIO,COL3_PIN)
#define COL4        HAL_GPIO_ReadPin(COL_GPIO,COL4_PIN)
#define COL5        HAL_GPIO_ReadPin(COL_GPIO,COL5_PIN)

//设置ROW为高电平
#define SET_ROW1_HIGH        HAL_GPIO_WritePin(ROW_GPIO,ROW1_PIN,GPIO_PIN_SET)
#define SET_ROW2_HIGH        HAL_GPIO_WritePin(ROW_GPIO,ROW2_PIN,GPIO_PIN_SET)
#define SET_ROW3_HIGH        HAL_GPIO_WritePin(ROW_GPIO,ROW3_PIN,GPIO_PIN_SET)
#define SET_ROW4_HIGH        HAL_GPIO_WritePin(ROW_GPIO,ROW4_PIN,GPIO_PIN_SET)

//设置ROW为低电平
#define SET_ROW1_LOW        HAL_GPIO_WritePin(ROW_GPIO,ROW1_PIN,GPIO_PIN_RESET)
#define SET_ROW2_LOW        HAL_GPIO_WritePin(ROW_GPIO,ROW2_PIN,GPIO_PIN_RESET)
#define SET_ROW3_LOW        HAL_GPIO_WritePin(ROW_GPIO,ROW3_PIN,GPIO_PIN_RESET)
#define SET_ROW4_LOW        HAL_GPIO_WritePin(ROW_GPIO,ROW4_PIN,GPIO_PIN_RESET)

// 高电平
#define KEY_DOWN  0
// 低电平
#define KEY_UP    1


//// 高电平
//#define KEY_DOWN	1
//// 低电平
//#define KEY_UP		0

void MK_init();

void MK_scan(uint16_t *state);

extern void MK_on_keyup(uint8_t row, uint8_t col);

extern void MK_on_keydown(uint8_t row, uint8_t col);

#endif