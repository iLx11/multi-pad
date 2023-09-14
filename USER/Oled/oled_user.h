#ifndef LEDEN_OLED_USER_H
#define LEDEN_OLED_USER_H

#include "oled.h"
#include "bmp.h"
#include "oled_96.h"

#define SIZE_42 sizeof(BMP1)
#define SIZE_96 sizeof(BMP20)
#define oled_42_l   72
#define oled_42_h   40
#define oled_42_x    0
#define oled_42_y    0
#define OLED_42_NUM 20

#define oled_96_l  128
#define oled_96_h   64
#define oled_96_x    0
#define oled_96_y    0
#define OLED_96_NUM  5

void oled_init_user();

void oled_show_user();

void load_show_menu(uint8_t menu_index);

void screen_effect(uint8_t row, uint8_t col, uint8_t mode, uint8_t direction);

#endif //LEDEN_OLED_USER_H
