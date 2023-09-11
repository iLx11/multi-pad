#include "encoder_user.h"
#include "oled_96.h"
#include "oled_user.h"

uint8_t menu_index = 0;
uint16_t cur_time = 0;
uint8_t change_flag = 0;

static void keyboard_menu_change();

// 函数重载
void HAL_IncTick(void) {
    cur_time > 6000 ? cur_time = 0 : cur_time++;
    if (cur_time > 500 && change_flag == 1) {
        keyboard_menu_change();
        change_flag = 0;
    }
}

uint32_t HAL_GetTick(void) {
    return cur_time;
}

/********************************************************************************
* 防抖函数
********************************************************************************/
void debounce_func(uint8_t encoder_index) {
    cur_time = 0;
    change_flag = 1;
    //OLED_ShowChar(10, 10, menu_index + 0x30, 24, 1, (encoder_index - 20) / 2);
}

/********************************************************************************
* 菜单更改
********************************************************************************/
static void keyboard_menu_change() {
    printf("menu_index -> %d\r", menu_index);
    if (menu_index % 2 == 0) Menu1();
    else Menu2();
}

void encoder_init_user() {
    EC11_EXTI_Init();
}

void encoder_callback(u_int8_t encoder_index) {
//    printf("encoder_index -> %d\r", encoder_index + 20);
    parse_json_value(encoder_index + 20);
}