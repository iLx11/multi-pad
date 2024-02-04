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
    lcd_show_pic_index(0, 0, 320, 172, (uint8_t *) gImage_1, 0);
    /*LCD_Init();
    LCD_Fill(0,0,LCD_W,LCD_H,CYAN);
    LCD_ShowPicture(0, 0, 172, 172, (uint8_t *) gImage_1);
    LCD_ShowPicture(59, 0, 172, 172, (uint8_t *) gImage_1);*/

}