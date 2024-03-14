#include "key_user.h"
#include "oled_user.h"

// ¼üÅÌ³õÊ¼×´Ì¬
static uint16_t state = 0xffff;

uint8_t is_key_holding = 0;
uint8_t holding_key_value = 200;

// ¼üÅÌ³õÊ¼»¯
void key_init_user() {
    matrix_init();
}

// ¼üÅÌÖ´ÐÐÉ¨Ãè
void key_scan_user() {
    matrix_scan();
}

// °´¼üÖ´ÐÐ
void key_up_callback(uint8_t row, uint8_t col) {
    uint8_t key_value = (col * ROW_NUM) + row;
    printf("key_value -> %d\n\r", (key_value << 1) + 1);
    parse_json_value((key_value << 1) + 1);
//    hid_buff_reset();
//    HAL_Delay(10);
}

void key_down_callback(uint8_t row, uint8_t col) {
    uint8_t key_value = (col * ROW_NUM) + row;
    printf("key_value -> %d\n\r", key_value << 1);
    parse_json_value(key_value << 1);
}

void key_hold_callback(uint8_t row, uint8_t col) {

}


