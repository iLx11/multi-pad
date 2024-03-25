/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/12/11 0:33
* @version: 1.0
* @description: 
********************************************************************************/


#ifndef MULTI_LCD_USER_H
#define MULTI_LCD_USER_H

#include "lcd_47.h"

void lcd_init_user(void);

void lcd_show_str(uint8_t* str);

void show_menu_color(uint8_t menu_index);

#endif //MULTI_LCD_USER_H
