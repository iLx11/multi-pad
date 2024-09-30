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
#include "lcd_user.h"

static p_menu_sign_t menu_sign_p = NULL;

/********************************************************************************
* 编码器初始化
********************************************************************************/
void encoder_init_user(void) {
    menu_sign_p = (p_menu_sign_t) malloc(sizeof(struct menu_sign));
    if(menu_sign_p == NULL) {
        free(menu_sign_p);
        return;
    }
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
* 菜单锁配置
********************************************************************************/
void set_menu_lock(uint8_t lock_value) {
    menu_sign_p->menu_change_lock = lock_value;
}

/********************************************************************************
* 菜单索引配置
********************************************************************************/
void set_menu_index(uint8_t index_value) {
    menu_sign_p->menu_index = index_value;
}

/********************************************************************************
* 获取菜单索引
********************************************************************************/
uint8_t get_menu_index(void) {
    return menu_sign_p->menu_index;
}

/********************************************************************************
* 防抖函数
********************************************************************************/
void debounce_func() {
    if(menu_sign_p->menu_change_lock == 0) {
        menu_sign_p->cur_time = 0;
        menu_sign_p->change_flag = 1;
        // 显示数字
        oled_42_show_num(menu_sign_p->menu_index, 1, 0);
    }
}

/********************************************************************************
* 菜单层级实质切换
********************************************************************************/
static void keyboard_menu_change() {
//    printf("menu_index -> %d\r", menu_index);
    if(menu_sign_p->pre_menu != menu_sign_p->menu_index) {
        menu_sign_p->pre_menu = menu_sign_p->menu_index;
        // 显示菜单
        load_menu(menu_sign_p->menu_index + 1);
    }
}

/********************************************************************************
* 函数重载防抖后执行菜单切换
********************************************************************************/
void HAL_IncTick(void) {
    menu_sign_p->cur_time > 6000 ? menu_sign_p->cur_time = 0 : menu_sign_p->cur_time++;
    if (menu_sign_p->cur_time > 300 && menu_sign_p->change_flag == 1) {
        keyboard_menu_change();
        menu_sign_p->change_flag = 0;
    }
}

uint32_t HAL_GetTick(void) {
    return menu_sign_p->cur_time;
}
/********************************************************************************
 *  每个编码器回调函数携带事件参数
 *  0 -> 按下
 *  1 -> 抬起
 *  2 -> 左旋
 *  3 -> 右旋
 *  4 -> 按下左旋
 *  5 -> 按下右旋
********************************************************************************/
// 编码器 1
void encoder1_callback(uint8_t encoder_value) {
//    if(menu_config_arr[menu_index + 1] == 0x00) {
    if(encoder_value == 4) {
        menu_sign_p->menu_index <= 0 ? menu_sign_p->menu_index = 9 : menu_sign_p->menu_index--;
        debounce_func();
        return;
    } else if(encoder_value == 5) {
        menu_sign_p->menu_index >= 9 ? menu_sign_p->menu_index = 0 : menu_sign_p->menu_index++;
        debounce_func();
        return;
    }
//    }
//    printf("encoder_value -> %d\n\r", encoder_value + 16);
    parse_json_value( encoder_value + 16);
}

// 编码器 2
void encoder2_callback(uint8_t encoder_value) {
//    printf("encoder_value -> %d\n\r", (ENCODER_EVENT_NUM * 1) + encoder_value + 16);
    parse_json_value( (ENCODER_EVENT_NUM * 1) + encoder_value + 16);
}

// 编码器 3
void encoder3_callback(uint8_t encoder_value) {
//    printf("encoder_value -> %d\n\r", (ENCODER_EVENT_NUM * 2) + encoder_value + 16);
    parse_json_value( (ENCODER_EVENT_NUM * 2) + encoder_value + 16);
}
