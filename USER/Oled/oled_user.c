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
    uint8_t cur_center = (col * ROW_NUM) + row;
    uint8_t level = 0, left = col - 1, top = row - 1, right = COL_NUM - (COL_NUM - col) + 1, bottom =
            ROW_NUM - (ROW_NUM - row) + 1;
    uint8_t pre = 0;
    uint8_t move_block = 0, left_store = 0, top_store = 0, right_store = 0, bottom_store = 0;
    OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                        white_background, 1, screen_index);
    HAL_Delay(30);
    OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                        oled_42_array[screen_index], 1, screen_index);
    if (pre != 0)
        OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                            oled_42_array[pre], 1, pre);
    for (; level < 4; level++) {

        move_block = 222;
        if (left >= 0 && left < COL_NUM) {
            move_block = screen_index = (left-- * ROW_NUM) + row;
            screen_shake(screen_index, &pre, mode);
        }
        // 左
        left_store = move_block;
        for(uint8_t i = 0; i < 4; i ++) {
            if((cur_center - move_block) % 5 != 0 && (move_block > 0 && move_block < 20) && move_block % 4 != 0) {
                if(move_block == 0) break;
                move_block -= 1;
                screen_shake(move_block, &pre, mode);
            }
            if(cur_center - left_store == 12 || cur_center - left_store == 15 || ((cur_center - left_store) % 3 != 0 && (left_store >= 0 && left_store < 20) && (left_store + 1) % 4 != 0)) {
                if(cur_center == 19 || cur_center == 15) continue;
                left_store += 1;
                screen_shake(left_store, &pre, mode);
            }
        }

        move_block = 222;
        if (top >= 0 && top < ROW_NUM) {
            move_block = screen_index = (col * ROW_NUM) + top--;
            screen_shake(screen_index, &pre, mode);
        }
        // 上
        top_store = move_block;
        for(uint8_t i = 0; i < 5; i ++) {
            if(((cur_center - 1) - move_block) % 5 != 0 && (move_block > 0 && move_block < 20)) {
                if(move_block == 0) break;
                move_block -= 4;
                screen_shake(move_block, &pre, mode);
            }
            if((top_store - (cur_center - 1)) % 3 != 0 && (top_store >= 0 && top_store < 20)) {
                top_store += 4;
                screen_shake(top_store, &pre, mode);
            }
        }


        move_block = 222;
        if (right >= 0 && right < COL_NUM) {
            move_block = screen_index = (right++ * ROW_NUM) + row;
            screen_shake(screen_index, &pre, mode);
        }
        // 右
        right_store = move_block;
        for(uint8_t i = 0; i < 4; i ++) {
            if(move_block - cur_center == 12 || move_block - cur_center == 15 || ((move_block - cur_center) % 3 != 0 && (move_block > 0 && move_block < 20) && move_block % 4 != 0)) {
                if(cur_center != 0) {
                    move_block -= 1;
                    screen_shake(move_block, &pre, mode);
                }
            }
            if((right_store - cur_center) % 5 != 0 && (right_store > 0 && right_store < 20) && (right_store + 1) % 4 != 0) {
                if(right_store == 0) break;
                right_store += 1;
                screen_shake(right_store, &pre, mode);
            }
        }

        // 下
        move_block = 222;
        if (bottom >= 0 && bottom < ROW_NUM) {
            move_block = screen_index = (col * ROW_NUM) + bottom++;
            screen_shake(screen_index, &pre, mode);
        }

        bottom_store = move_block;
        for(uint8_t i = 0; i < 5; i ++) {
            if(((cur_center + 1) - move_block) % 3 != 0 && (move_block > 0 && move_block < 20)) {
                if(move_block == 0) break;
                move_block -= 4;
                screen_shake(move_block, &pre, mode);
            }
            if((bottom_store - (cur_center + 1)) % 5 != 0 && (bottom_store > 0 && bottom_store < 20)) {
                if(bottom_store == 0) break;
                bottom_store += 4;
                screen_shake(bottom_store, &pre, mode);
            }
        }
    }
    OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                        oled_42_array[pre], 1, pre);
    if(repeat != 0) {
        HAL_Delay(40);
        screen_effect_two(row, col, mode, repeat-1);
    }
}

/********************************************************************************
* 屏幕切换
********************************************************************************/
static void screen_shake(uint8_t screen_index, uint8_t *pre, uint8_t mode) {
    if(screen_index > 20) return ;
    if (mode == 0) {
        OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                            white_background, 1, screen_index);
    } else if (mode == 1) {
        OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                            black_background, 1, screen_index);
    }
    HAL_Delay(5);
    if (pre != 0)
        OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                            oled_42_array[(*pre)], 1, (*pre));
    (*pre) = screen_index;
}

