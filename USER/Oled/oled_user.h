#ifndef LEDEN_OLED_USER_H
#define LEDEN_OLED_USER_H

//#include "bmp.h"
#include "string.h"
#include "stm32f1xx_hal.h"

#define SIZE_42    360
#define SIZE_96    999
#define oled_42_l   72
#define oled_42_h   40
#define oled_42_x    0
#define oled_42_y    0
#define OLED_42_NUM 2

#define oled_96_l  128
#define oled_96_h   64
#define oled_96_x    0
#define oled_96_y    0
#define OLED_96_NUM  5

void oled_init_user();

void load_show_menu(uint8_t menu_index);

void show_menu_oled(uint8_t menu_index);

#endif //LEDEN_OLED_USER_H
