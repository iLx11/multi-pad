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
    lcd_fill(0,0,LCD_WIDTH,LCD_HEIGHT,WHITE, 0);
//    lcd_show_pic_index(0, 0, 40, 86, (uint8_t *) gImage_1, 0);
    lcd_show_string(2, 3, "sdfs", BLACK, WHITE, 32, 0, 0);
    lcd_show_string(2, 3, "ASDFASDFA", BLACK, WHITE, 32, 0, 0);
    lcd_show_string(2, 3, "AYUIOYUIOYUIO", BLACK, WHITE, 32, 0, 0);
    lcd_show_string(2, 3, "XZCVZXCVZXCV", BLACK, WHITE, 32, 0, 0);

}