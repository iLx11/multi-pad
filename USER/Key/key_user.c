#include "key_user.h"
#include "oled_user.h"

// ¼üÅÌ³õÊ¼×´Ì¬
static uint16_t state = 0xffff;

uint8_t is_key_holding = 0;
uint8_t holding_key_value = 200;

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
//    hid_buff_reset();
//    HAL_Delay(10);
}

void MK_on_keydown(uint8_t row, uint8_t col) {
    uint8_t key_value = (col * ROW_NUM) + row;
//    screen_effect_two(row, col, 0, 0);
    printf("key_value -> %d\n", key_value);
//    parse_json_value(key_value);
}

// Ñ­»·Ö´ÐÐ
void holding_key_execute() {
    if(is_key_holding == 1) {
        parse_json_value(holding_key_value);
    }
}

void key_hold_callback(uint8_t row, uint8_t col) {
    printf("%d is holding\n\r", (col * ROW_NUM) + row);
    if(is_key_holding == 1) {
        is_key_holding = 0;
        return;
    }
    uint8_t key_value = (col * ROW_NUM) + row;
    if(is_key_holding == 0) {
        is_key_holding = 1;
        holding_key_value = key_value;
    }
}


