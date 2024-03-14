/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/8/20 10:56
* @version: 1.0
* @description: ���������������
********************************************************************************/


#include "matrix_key.h"

uint16_t key_state = STATE;

/********************************************************************************
* �жϼ���̧��ͷ���
********************************************************************************/
#define IS_KEY_UP(row, col) ((key_state >> row * COL_NUM + col) & 0x01) == KEY_UP
#define IS_KEY_DOWN(row, col) ((key_state >> row * COL_NUM + col) & 0x01) == KEY_DOWN

/********************************************************************************
* ���ü�¼���̵���һ��״̬
********************************************************************************/
#define SET_KEY_UP(row, col) key_state |= (0x01 << (row * COL_NUM + col))
#define SET_KEY_DOWN(row, col) key_state &= ~(0x01 << (row * COL_NUM + col))

/********************************************************************************
* ��ʼ��
********************************************************************************/
void matrix_init() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    ENABLE_ROW_RCC;
    GPIO_InitStruct.Pin = ROW1_PIN | ROW2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ROW_GPIO, &GPIO_InitStruct);

    ENABLE_COL_RCC;
    GPIO_InitStruct.Pin = COL1_PIN | COL2_PIN | COL3_PIN | COL4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(COL_GPIO, &GPIO_InitStruct);

}

/********************************************************************************
* ����ɨ��
********************************************************************************/
void matrix_scan() {
    for(uint8_t row = 0; row < ROW_NUM; row ++) {
        set_row_low(row);
        for(uint8_t col = 0; col < COL_NUM; col ++) {
            if(get_col_state(col) == KEY_DOWN && IS_KEY_UP(row, col)) {
                // ��¼��ǰ���µ�״̬
                SET_KEY_DOWN(row, col);
                // ִ�а��»ص�����
                key_down_callback(row, col);
            }else if(get_col_state(col) == KEY_UP && IS_KEY_DOWN(row, col)) {
                // ��¼��ǰ���µ�״̬
                SET_KEY_UP(row, col);
                // ִ��̧��ص�����
                key_up_callback(row, col);
            }
            // ���̰�ס���
            key_hold_check(row, col);
        }
    }
}

/********************************************************************************
* ����
********************************************************************************/
static void set_row_low(uint8_t row) {
    if(row == 0) {SET_ROW1_LOW; SET_ROW2_HIGH;}
    if(row == 1) {SET_ROW1_HIGH; SET_ROW2_LOW;}
}

/********************************************************************************
* ��ȡ col ״̬
********************************************************************************/
static uint8_t get_col_state(uint8_t col) {
    if(col == 0) return GET_COL1_STATE;
    if(col == 1) return GET_COL2_STATE;
    if(col == 2) return GET_COL3_STATE;
    if(col == 3) return GET_COL4_STATE;
}

/********************************************************************************
* ���̰�ס�ж�
********************************************************************************/
void key_hold_check(uint8_t row, uint8_t col) {
    uint8_t key_hold_count = 0;
    while(get_col_state(col) == KEY_DOWN) {
        key_hold_count ++;
        HAL_Delay(10);
        if(key_hold_count > 50) {
            // ���ûص�����
            key_hold_callback(row, col);
        }
    }
}