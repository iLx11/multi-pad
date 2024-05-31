/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/30 8:25
* @version: 1.0
* @description: 
********************************************************************************/


#ifndef MULTI_ENCODER_USER_H
#define MULTI_ENCODER_USER_H

#include <stdio.h>
#include "jsmn_user.h"
#include "oled_user.h"

#define ENCODER_EVENT_NUM 6

// 菜单信号结构体类型
typedef struct menu_sign {
    // 切换菜单索引
    uint8_t menu_index;
    // 菜单更改标志
    uint8_t change_flag;
    // 菜单更改锁
    uint8_t menu_change_lock;
    // 前加载菜单，防止重复加载统一菜单
    uint8_t pre_menu;
    // 防抖阈值时间
    uint16_t cur_time;
}* p_menu_sign_t;

void encoder_init_user(void);

void encoder_scan_user(void);

void debounce_func();

void set_menu_lock(uint8_t lock_value);

void set_menu_index(uint8_t index_value);

uint8_t get_menu_index(void);

static void keyboard_menu_change();

#endif //MULTI_ENCODER_USER_H
