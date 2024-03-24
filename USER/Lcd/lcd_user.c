/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/12/11 0:33
* @version: 1.0
* @description: 
********************************************************************************/


#include "lcd_user.h"
#include "pic.h"

void lcd_init_user(void) {
    lcd_47_init();
    lcd_show_str("LCD OK");
}

void lcd_show_str(uint8_t* str) {
    lcd_fill(0,0,LCD_WIDTH,LCD_HEIGHT,WHITE, 0);
    lcd_show_string(10, 10, str, BLACK, WHITE, 32, 0, 0);
}