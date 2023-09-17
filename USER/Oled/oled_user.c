#include "oled_user.h"
#include "oled.h"
#include "flash_user.h"
#include "MatrixKeys.h"

extern uint8_t menu_change_lock;

void load_menu_from_flash(uint8_t folderIndex);

void oled_show_pic(uint8_t menu_index);

static void screen_shake(uint8_t screen_index, uint8_t *pre, uint8_t mode);

// 图片存储数组
uint8_t oled_42_array[OLED_42_NUM][SIZE_42];
uint8_t oled_96_array[OLED_96_NUM][SIZE_96];

// 乱序数组
uint8_t random_refresh[3][20] = {7, 14, 17, 10, 11, 3, 18, 5, 13, 15, 9, 1, 0, 2, 6, 8, 16, 4, 19, 12,
                                 9, 12, 6, 8, 4, 19, 0, 7, 17, 3, 13, 15, 1, 16, 10, 11, 2, 14, 5, 18,
                                 3, 18, 15, 13, 16, 10, 4, 1, 0, 14, 7, 17, 11, 5, 9, 19, 12, 6, 2, 8};

// 黑白屏幕数组
static unsigned char black_background[360] = {0x00};
static unsigned char white_background[360] = {
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

// 初始化
void oled_init_user() {
    OLED_42_Init();
    OLED_92_Init();
//    OLED_42_ShowPicture(0,0,72,42,BMP0,1,1);
    load_show_menu(0);
}

/********************************************************************************
* 菜单读取后显示
********************************************************************************/
void load_show_menu(uint8_t menu_index) {
    menu_change_lock = 1;
    load_menu_from_flash(menu_index);
    oled_show_pic(menu_index);
    menu_change_lock = 0;
}

/********************************************************************************
* 从 flash 加载图片
********************************************************************************/
void load_menu_from_flash(uint8_t folderIndex) {
    for (int i = 0; i < 20; i++) {
        read_from_flash(folderIndex, i, oled_42_array[i], SIZE_42);
        if (i < OLED_96_NUM)
            read_from_flash(folderIndex, i + OLED_42_NUM, oled_96_array[i], SIZE_96);
    }
}

/********************************************************************************
* 显示图片
********************************************************************************/
void oled_show_pic(uint8_t menu_index) {
    for (int i = 0; i < OLED_42_NUM; i++) {
        OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                            oled_42_array[random_refresh[menu_index % 3][i]], 1, random_refresh[menu_index % 3][i]);
        if (i < OLED_96_NUM)
            OLED_92_ShowPicture(oled_96_x, oled_96_y, oled_96_l, oled_96_h, oled_96_array[i], 1, i);
    }
}

/********************************************************************************
* 屏幕动效（十字特效）
********************************************************************************/
void screen_effect(uint8_t row, uint8_t col, uint8_t mode, uint8_t repeat) {
    if (repeat < 0 || repeat > 10) return;
    uint8_t screen_index = (col * ROW_NUM) + row;
    uint8_t level = 0, left = col - 1, top = row - 1, right = COL_NUM - (COL_NUM - col) + 1, bottom =
            ROW_NUM - (ROW_NUM - row) + 1;
    uint8_t pre = 0;
    OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                        white_background, 1, screen_index);
    HAL_Delay(30);
    OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                        oled_42_array[screen_index], 1, screen_index);
    if (pre != 0)
        OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                            oled_42_array[pre], 1, pre);
    for (; level < 4; level++) {
        if (left >= 0 && left < COL_NUM) {
            screen_index = (left-- * ROW_NUM) + row;
            screen_shake(screen_index, &pre, mode);
        }
        if (top >= 0 && top < ROW_NUM) {
            screen_index = (col * ROW_NUM) + top--;
            screen_shake(screen_index, &pre, mode);
        }
        if (right >= 0 && right < COL_NUM) {
            screen_index = (right++ * ROW_NUM) + row;
            screen_shake(screen_index, &pre, mode);
        }
        if (bottom >= 0 && bottom < ROW_NUM) {
            screen_index = (col * ROW_NUM) + bottom++;
            screen_shake(screen_index, &pre, mode);
        }
    }
    OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                        oled_42_array[pre], 1, pre);
    if(repeat != 0) {
        HAL_Delay(40);
        screen_effect(row, col, mode, repeat-1);
    }
}

/********************************************************************************
* 屏幕动效（波浪）
********************************************************************************/
void screen_effect_two(uint8_t row, uint8_t col, uint8_t mode, uint8_t repeat) {
    if (repeat < 0 || repeat > 10) return;
    uint8_t screen_index = (col * ROW_NUM) + row;
    uint8_t level = 0, left = col - 1, top = row - 1, right = COL_NUM - (COL_NUM - col) + 1, bottom =
            ROW_NUM - (ROW_NUM - row) + 1;
    uint8_t pre = 0;
    uint8_t left_up_y = 0, left_up_x = 0;
    OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                        white_background, 1, screen_index);
    HAL_Delay(30);
    OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                        oled_42_array[screen_index], 1, screen_index);
    if (pre != 0)
        OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                            oled_42_array[pre], 1, pre);
    for (; level < 4; level++) {

        left_up_x = 0;
        if (left >= 0 && left < COL_NUM) {
            screen_index = (left-- * ROW_NUM) + row;
            left_up_x = left;
            screen_shake(screen_index, &pre, mode);
        }
        left_up_y = 0;
        if (top >= 0 && top < ROW_NUM) {
            screen_index = (col * ROW_NUM) + top--;
            left_up_y = top;
            screen_shake(screen_index, &pre, mode);
        }
//        while(left_up_y-- <= left_up_x)
 /*           OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                                white_background, 1, left_pre);
            HAL_Delay(10);
            OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                                oled_42_array[left_pre], 1, left_pre);*/


        if (right >= 0 && right < COL_NUM) {
            screen_index = (right++ * ROW_NUM) + row;
            screen_shake(screen_index, &pre, mode);
        }
        if (bottom >= 0 && bottom < ROW_NUM) {
            screen_index = (col * ROW_NUM) + bottom++;
            screen_shake(screen_index, &pre, mode);
        }
    }
    OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                        oled_42_array[pre], 1, pre);
    screen_effect(row, col, mode, repeat-1);
    HAL_Delay(40);
}
/********************************************************************************
* 屏幕切换
********************************************************************************/

static void screen_shake(uint8_t screen_index, uint8_t *pre, uint8_t mode) {
    if (mode == 0) {
        OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                            white_background, 1, screen_index);
    } else if (mode == 1) {
        OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                            black_background, 1, screen_index);
    }
    HAL_Delay(30);
    if (pre != 0)
        OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                            oled_42_array[(*pre)], 1, (*pre));
    (*pre) = screen_index;
}

