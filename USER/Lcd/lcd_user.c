/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/12/11 0:33
* @version: 1.0
* @description: 
********************************************************************************/


#include <malloc.h>
#include "lcd_user.h"
#include "flash_user.h"

void lcd_init_user(void) {
    lcd_47_init();
    lcd_show_str("LCD OK");
}

/********************************************************************************
* ÏÔÊ¾×Ö·û´®
********************************************************************************/
void lcd_show_str(uint8_t* str) {
    lcd_fill(0,0,LCD_WIDTH,LCD_HEIGHT,WHITE, 0);
    lcd_show_string(10, 10, str, BLACK, WHITE, 32, 0, 0);
}

/********************************************************************************
* ÏÔÊ¾µ¥²ãµÄÍ¼Æ¬
********************************************************************************/
void show_menu_color(uint8_t menu_index) {
    uint8_t *image_arr = (uint8_t *) malloc(sizeof(uint8_t) * 3200);
    for (uint8_t i = 0; i < 35; i++) {
        if(i != 34) {
            // ¶ÁÈ¡Í¼Æ¬
            read_color_screen(menu_index, i * 3200, image_arr, 3200);
            // ÏÔÊ¾Í¼Æ¬
            lcd_show_pic_index(0, i * 5, 320, 5, image_arr, 0);
        } else {
            read_color_screen(menu_index, i * 3200, image_arr, 1280);
            lcd_show_pic_index(0, i * 5, 320, 2, image_arr, 0);
        }
    }
    free(image_arr);
    image_arr = NULL;
}