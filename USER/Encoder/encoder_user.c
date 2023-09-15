#include "encoder_user.h"
#include "oled_user.h"
#include "numbmp.h"

uint8_t menu_index = 0;
uint16_t cur_time = 0;
uint8_t change_flag = 0;
uint8_t menu_change_lock = 0;

uint8_t cur_menu = 0;

extern uint8_t oled_96_array[OLED_96_NUM][SIZE_96];

static void keyboard_menu_change();

// 初始化
void encoder_init_user() {
    EC11_EXTI_Init();
}
/********************************************************************************
* 编码器中断回调并解析
********************************************************************************/
void encoder_callback(u_int8_t encoder_index) {
//    printf("encoder_index -> %d\r", encoder_index + 20);
    parse_json_value(encoder_index + 20);
}

/********************************************************************************
* 防抖函数
********************************************************************************/
void debounce_func(uint8_t encoder_index) {
    if(menu_change_lock == 0) {
        cur_time = 0;
        change_flag = 1;
//    OLED_ShowChar(10, 10, menu_index + 0x30, 24, 1, (encoder_index - 20) / 2);
        OLED_92_ShowPicture(oled_96_x, oled_96_y, oled_96_l, oled_96_h, num_bmp[menu_index], 1, (encoder_index - 20) / 2);
    }
}

/********************************************************************************
* 菜单层级实质切换
********************************************************************************/
static void keyboard_menu_change() {
    printf("menu_index -> %d\r", menu_index);
    cur_menu == 0 ? (cur_menu = 1) : (cur_menu = 0);
    // 加载图片并显示
    load_show_menu(cur_menu);
    // 加载当前层级的命令

}

/********************************************************************************
* 函数重载防抖后执行菜单切换
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
