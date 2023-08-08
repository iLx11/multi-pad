#include "oled_user.h"
#include "oled.h"

void oled_init_user() {
    OLED_Init();
    OLED_ColorTurn(0);//0������ʾ��1 ��ɫ��ʾ
    OLED_DisplayTurn(0);//0������ʾ 1 ��Ļ��ת��ʾ
    OLED_Refresh();
}

void oled_show_user() {
    OLED_Clear();
    OLED_ShowChinese(0, 0, 0, 16, 1);//��
    OLED_Refresh();
}