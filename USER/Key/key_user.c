#include "key_user.h"
#include "oled_user.h"

// ¼üÅÌ³õÊ¼×´Ì¬
static uint16_t state = 0xffff;

// ¼üÅÌ³õÊ¼»¯
void key_init_user() {
    MK_init();
}

// ¼üÅÌÖ´ÐÐÉ¨Ãè
void key_scan_user() {
    MK_scan(&state);
}

// °´¼üÖ´ÐÐ
void MK_on_keyup(uint8_t row, uint8_t col) {
    hid_buff_reset();
    HAL_Delay(10);
}

void MK_on_keydown(uint8_t row, uint8_t col) {
    uint8_t key_value = (col * ROW_NUM) + row;
    screen_effect(row, col, 0, 0);
//    screen_effect_two(row, col, 0, 0);
    printf("key_value -> %d\n", key_value);
    parse_json_value(key_value);
}


