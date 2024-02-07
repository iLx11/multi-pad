#include "key_user.h"
#include "oled_user.h"

// ���̳�ʼ״̬
static uint16_t state = 0xffff;

uint8_t is_key_holding = 0;
uint8_t holding_key_value = 200;

// ���̳�ʼ��
void key_init_user() {
    MK_init();
}

// ����ִ��ɨ��
void key_scan_user() {
    MK_scan(&state);
}

// ����ִ��
void MK_on_keyup(uint8_t row, uint8_t col) {
//    hid_buff_reset();
//    HAL_Delay(10);
}

void MK_on_keydown(uint8_t row, uint8_t col) {
    uint8_t key_value = (col * ROW_NUM) + row;
    printf("key_value -> %d\n", key_value);
    parse_json_value(key_value);
}

void key_hold_callback(uint8_t row, uint8_t col) {

}


