#include <malloc.h>
#include "oled_user.h"
#include "flash_user.h"
#include "oled_42.h"
#include "number_pic.h"

extern uint8_t menu_change_lock;

// 初始化
void oled_init_user() {
    oled_42_init();
}

/********************************************************************************
* 显示当前层级的图片
********************************************************************************/
void show_menu_oled(uint8_t menu_index) {
    menu_change_lock = 1;
    // 读取图片
    uint8_t* tempArr = (uint8_t *) malloc(sizeof (uint8_t) * 720);
    load_menu_from_flash(menu_index, tempArr, 720, 1);
    oled_42_show_pic_index(0, 0, 72, 40, tempArr,  1, 0);
    oled_42_show_pic_index(0, 0, 72, 40, tempArr + 360,  1, 1);
    free(tempArr);
    tempArr = NULL;
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
