#include "key_user.h"
#include "oled_user.h"
#include "usbd_cdc_if.h"
#include "screen_readback.h"
#include "pet_mode.h"

// 键盘初始状态
static uint8_t pet_claimed;

static uint8_t holding_flag = 0;

// 键盘初始化
void key_init_user() {
    matrix_init();
}

// 键盘执行扫描
void key_scan_user() {
    matrix_scan();
}

/********************************************************************************
* 按键抬起回调
********************************************************************************/
void key_up_callback(uint8_t row, uint8_t col) {
    uint8_t key_value = (col * ROW_NUM) + row;
//    printf("key_value -> %d\n\r", (key_value << 1) + 1);
    button_readback_record(key_value, false);
    if (pet_claimed & (1u << key_value)) {
        pet_claimed &= ~(1u << key_value);
        return;
    }
    parse_json_value((key_value << 1) + 1);
    holding_flag = 0;
}

/********************************************************************************
* 按键按下回调
********************************************************************************/
void key_down_callback(uint8_t row, uint8_t col) {
    uint8_t key_value = (col * ROW_NUM) + row;
//    printf("key_value -> %d\n\r", key_value << 1);
    button_readback_record(key_value, true);
    if (pet_mode_key_down(key_value)) {
        pet_claimed |= 1u << key_value;
        return;
    }
    parse_json_value(key_value << 1);
}

/********************************************************************************
* 按键按住回调
********************************************************************************/
void key_hold_callback(uint8_t row, uint8_t col) {
    uint8_t key_value = (col * ROW_NUM) + row;
    if (pet_claimed & (1u << key_value)) return;
    HAL_Delay(10);
    if (holding_flag == 0) {
        holding_flag = 1;
        parse_json_value(key_value << 1);
    }
}
