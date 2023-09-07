#ifndef __MATRIX_KEYS__
#define __MATRIX_KEYS__

#include "stm32f1xx.h"

//����������
#define  ROW_NUM  4
#define  COL_NUM  5
//GPIO��Ҫ�Ĳ���
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

//�����ò�ͬ�а��µ������Ȼ��ɨ���е���Ϊ1������¾Ͱ��¡�
//��ȡCOL������ƽ
#define COL1        HAL_GPIO_ReadPin(COL_GPIO,COL1_PIN)
#define COL2        HAL_GPIO_ReadPin(COL_GPIO,COL2_PIN)
#define COL3        HAL_GPIO_ReadPin(COL_GPIO,COL3_PIN)
#define COL4        HAL_GPIO_ReadPin(COL_GPIO,COL4_PIN)
#define COL5        HAL_GPIO_ReadPin(COL_GPIO,COL5_PIN)

//����ROWΪ�ߵ�ƽ
#define SET_ROW1_HIGH        HAL_GPIO_WritePin(ROW_GPIO,ROW1_PIN,GPIO_PIN_SET)
#define SET_ROW2_HIGH        HAL_GPIO_WritePin(ROW_GPIO,ROW2_PIN,GPIO_PIN_SET)
#define SET_ROW3_HIGH        HAL_GPIO_WritePin(ROW_GPIO,ROW3_PIN,GPIO_PIN_SET)
#define SET_ROW4_HIGH        HAL_GPIO_WritePin(ROW_GPIO,ROW4_PIN,GPIO_PIN_SET)

//����ROWΪ�͵�ƽ
#define SET_ROW1_LOW        HAL_GPIO_WritePin(ROW_GPIO,ROW1_PIN,GPIO_PIN_RESET)
#define SET_ROW2_LOW        HAL_GPIO_WritePin(ROW_GPIO,ROW2_PIN,GPIO_PIN_RESET)
#define SET_ROW3_LOW        HAL_GPIO_WritePin(ROW_GPIO,ROW3_PIN,GPIO_PIN_RESET)
#define SET_ROW4_LOW        HAL_GPIO_WritePin(ROW_GPIO,ROW4_PIN,GPIO_PIN_RESET)

// �ߵ�ƽ
#define KEY_DOWN  0
// �͵�ƽ
#define KEY_UP    1


//// �ߵ�ƽ
//#define KEY_DOWN	1
//// �͵�ƽ
//#define KEY_UP		0

void MK_init();

void MK_scan(uint16_t *state);

extern void MK_on_keyup(uint8_t row, uint8_t col);

extern void MK_on_keydown(uint8_t row, uint8_t col);

#endif