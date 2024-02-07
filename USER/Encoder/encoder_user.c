/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/30 8:25
* @version: 1.0
* @description: 
********************************************************************************/


#include "encoder_user.h"
#include "encoder1.h"
#include "encoder2.h"
#include "encoder3.h"
#include "oled_user.h"
#include "oled_42.h"

uint8_t menu_index = 0;
uint16_t cur_time = 0;
uint8_t change_flag = 0;
uint8_t menu_change_lock = 0;

uint8_t pre_menu = 0;

static void keyboard_menu_change();

void encoder_init_user(void) {
    encoder1_init();
    encoder2_init();
    encoder3_init();
}

void encoder_scan_user(void) {
    encoder1_scan();
    encoder2_scan();
    encoder3_scan();
}

/********************************************************************************
* ±àÂëÆ÷ÖÐ¶Ï»Øµ÷²¢½âÎö
********************************************************************************/
void encoder_callback(u_int8_t encoder_index) {
//    printf("encoder_index -> %d\r", encoder_index + 20);
    parse_json_value(encoder_index + 20);
}

/********************************************************************************
* ·À¶¶º¯Êý
********************************************************************************/
void debounce_func(uint8_t encoder_index) {
    if(menu_change_lock == 0) {
        cur_time = 0;
        change_flag = 1;
    }
}

/********************************************************************************
* ²Ëµ¥²ã¼¶ÊµÖÊÇÐ»»
********************************************************************************/
static void keyboard_menu_change() {
    printf("menu_index -> %d\r", menu_index);
    if(pre_menu != menu_index) {
        pre_menu = menu_index;
        // ¼ÓÔØÍ¼Æ¬²¢ÏÔÊ¾
        load_show_menu(menu_index);
        // ¼ÓÔØµ±Ç°²ã¼¶µÄÃüÁî
        load_parse_key(menu_index);
    }
}

/********************************************************************************
* º¯ÊýÖØÔØ·À¶¶ºóÖ´ÐÐ²Ëµ¥ÇÐ»»
********************************************************************************/
void HAL_IncTick(void) {
    cur_time > 6000 ? cur_time = 0 : cur_time++;
    if (cur_time > 300 && change_flag == 1) {
        keyboard_menu_change();
        change_flag = 0;
    }
}

uint32_t HAL_GetTick(void) {
    return cur_time;
}

// ±àÂëÆ÷ 1
void encoder1_callback(uint8_t encoder_value) {
    printf("encoder_value -> %d\n\r", encoder_value);
}

// ±àÂëÆ÷ 2
void encoder2_callback(uint8_t encoder_value) {
    printf("encoder_value -> %d\n\r", encoder_value);
}

// ±àÂëÆ÷ 3
void encoder3_callback(uint8_t encoder_value) {
    printf("encoder_value -> %d\n\r", encoder_value);
}
