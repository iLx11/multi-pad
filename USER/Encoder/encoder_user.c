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
#include "oled_42.h"

uint8_t menu_index = 0;
uint16_t cur_time = 0;
uint8_t change_flag = 0;
uint8_t menu_change_lock = 0;

uint8_t pre_menu = 0;
// �˵���������
extern uint8_t menu_config_arr[31];

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
* ��������
********************************************************************************/
void debounce_func(void) {
    if(menu_change_lock == 0) {
        cur_time = 0;
        change_flag = 1;
        // ��ʾ�л�����
        oled_42_show_num(menu_index, 1, 0);
    }
}

/********************************************************************************
* �˵��㼶ʵ���л�
********************************************************************************/
static void keyboard_menu_change() {
    printf("menu_index -> %d\r", menu_index);
    if(pre_menu != menu_index) {
        pre_menu = menu_index;
        // ��ʾ�˵�
        load_menu(menu_index + 1);
    }
}

/********************************************************************************
* �������ط�����ִ�в˵��л�
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
/*
 *  ÿ���������ص�����Я���¼�����
 *  0 -> ����
 *  1 -> ̧��
 *  2 -> ����
 *  3 -> ����
 *  4 -> ��������
 *  5 -> ��������
 * */
// ������ 1
void encoder1_callback(uint8_t encoder_value) {
    if(menu_config_arr[menu_index + 1] == 0x00) {
        if(encoder_value == 4) {
            menu_index <= 0 ? menu_index = 9 : menu_index--;
        } else if(encoder_value == 5) {
            menu_index >= 9 ? menu_index = 0 : menu_index++;
        }
        debounce_func();
        return;
    }
    printf("encoder_value -> %d\n\r", encoder_value + 16);
    parse_json_value( encoder_value + 16);
}

// ������ 2
void encoder2_callback(uint8_t encoder_value) {
    printf("encoder_value -> %d\n\r", (ENCODER_EVENT_NUM * 1) + encoder_value + 16);
    parse_json_value( (ENCODER_EVENT_NUM * 1) + encoder_value + 16);
}

// ������ 3
void encoder3_callback(uint8_t encoder_value) {
    printf("encoder_value -> %d\n\r", (ENCODER_EVENT_NUM * 2) + encoder_value + 16);
    parse_json_value( (ENCODER_EVENT_NUM * 2) + encoder_value + 16);
}
