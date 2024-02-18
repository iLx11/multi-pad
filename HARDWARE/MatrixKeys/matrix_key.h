#ifndef __MATRIX_KEYS__
#define __MATRIX_KEYS__

#include "stm32f1xx.h"

//设置行列数
#define  ROW_NUM  2
#define  COL_NUM  4

#define STATE 0xff

#define ROW_GPIO     GPIOB
#define COL_GPIO     GPIOE

#define ENABLE_ROW_RCC __HAL_RCC_GPIOB_CLK_ENABLE()
#define ENABLE_COL_RCC __HAL_RCC_GPIOE_CLK_ENABLE()

#define ROW1_PIN    GPIO_PIN_10
#define ROW2_PIN    GPIO_PIN_11

#define COL1_PIN    GPIO_PIN_12
#define COL2_PIN    GPIO_PIN_13
#define COL3_PIN    GPIO_PIN_14
#define COL4_PIN    GPIO_PIN_15

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

#define GET_COL1_STATE HAL_GPIO_ReadPin(COL_GPIO, COL1_PIN);
#define GET_COL2_STATE HAL_GPIO_ReadPin(COL_GPIO, COL2_PIN);
#define GET_COL3_STATE HAL_GPIO_ReadPin(COL_GPIO, COL3_PIN);
#define GET_COL4_STATE HAL_GPIO_ReadPin(COL_GPIO, COL4_PIN)

// 高电平
#define KEY_DOWN  0
// 低电平
#define KEY_UP    1

void matrix_init();

void matrix_scan();

static void set_row_low(uint8_t row);

// 获取列的状态
static uint8_t get_col_state(uint8_t col);

// 外部重写按键的回调函数
__weak void key_down_callback(uint8_t row, uint8_t col);

__weak void key_up_callback(uint8_t row, uint8_t col);

__weak void key_hold_callback(uint8_t row, uint8_t col);

static void key_hold_check(uint8_t row, uint8_t col);

#endif