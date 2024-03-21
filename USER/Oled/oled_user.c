#include <malloc.h>
#include "oled_user.h"
#include "flash_user.h"
#include "oled_42.h"
#include "number_pic.h"

extern uint8_t menu_change_lock;

// 图片存储数组
uint8_t oled_42_array[OLED_42_NUM][SIZE_42];

// 初始化
void oled_init_user() {
    oled_42_init();
    load_show_menu(0);
}

/********************************************************************************
* 菜单读取后显示
********************************************************************************/
void load_show_menu(uint8_t menu_index) {
    menu_change_lock = 1;
//    uint8_t* flash_pic_data = (uint8_t *) malloc(sizeof (uint8_t) * 720);
    // 读取与赋值图片

    // 显示图片
//    free(flash_pic_data);
    menu_change_lock = 0;
}


/********************************************************************************
* 显示数字
********************************************************************************/
void oled_42_show_num(uint8_t num, uint8_t mode, uint8_t index) {
    if(num > 9) return ;
    if(mode == 0)
        oled_42_show_pic_index(0,0,72,40, number_array_1_0[num],1, index);
    else if(mode == 1)
        oled_42_show_pic_index(0,0,72,40, number_array_1_1[num],1, index);
    else if(mode == 2)
        oled_42_show_pic_index(0,0,72,40, number_array_2_0[num],1, index);
    else
        oled_42_show_pic_index(0,0,72,40, number_array_2_1[num],1, index);
}
