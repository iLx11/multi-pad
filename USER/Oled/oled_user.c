#include "oled_user.h"
#include "oled.h"

void oled_init_user() {
    OLED_Init();
    OLED_ColorTurn(0);//0正常显示，1 反色显示
    OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
    OLED_Refresh();
}

void oled_show_user() {
    OLED_Clear();
    OLED_ShowChinese(0, 0, 0, 16, 1);//中
    OLED_Refresh();
}