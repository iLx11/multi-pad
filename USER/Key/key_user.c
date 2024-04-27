#include "key_user.h"
#include "oled_user.h"
#include "usbd_cdc_if.h"

// ���̳�ʼ״̬
static uint16_t state = 0xffff;

uint8_t holding_flag = 0;

// ���̳�ʼ��
void key_init_user() {
    matrix_init();
}

// ����ִ��ɨ��
void key_scan_user() {
    matrix_scan();
}

/********************************************************************************
* ����̧��ص�
********************************************************************************/
void key_up_callback(uint8_t row, uint8_t col) {
    uint8_t key_value = (col * ROW_NUM) + row;
//    printf("key_value -> %d\n\r", (key_value << 1) + 1);
    parse_json_value((key_value << 1) + 1);
    holding_flag = 0;
}

/********************************************************************************
* �������»ص�
********************************************************************************/
void key_down_callback(uint8_t row, uint8_t col) {
    uint8_t key_value = (col * ROW_NUM) + row;
//    printf("key_value -> %d\n\r", key_value << 1);
    parse_json_value(key_value << 1);
}

/********************************************************************************
* ������ס�ص�
********************************************************************************/
void key_hold_callback(uint8_t row, uint8_t col) {
    HAL_Delay(10);
    if (holding_flag == 0) {
        holding_flag = 1;
        uint8_t key_value = (col * ROW_NUM) + row;
        parse_json_value(key_value << 1);
    }
}
