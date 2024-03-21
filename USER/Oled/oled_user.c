#include <malloc.h>
#include "oled_user.h"
#include "flash_user.h"
#include "oled_42.h"
#include "number_pic.h"

extern uint8_t menu_change_lock;

// ͼƬ�洢����
uint8_t oled_42_array[OLED_42_NUM][SIZE_42];

// ��ʼ��
void oled_init_user() {
    oled_42_init();
    load_show_menu(0);
}

/********************************************************************************
* �˵���ȡ����ʾ
********************************************************************************/
void load_show_menu(uint8_t menu_index) {
    menu_change_lock = 1;
//    uint8_t* flash_pic_data = (uint8_t *) malloc(sizeof (uint8_t) * 720);
    // ��ȡ�븳ֵͼƬ

    // ��ʾͼƬ
//    free(flash_pic_data);
    menu_change_lock = 0;
}


/********************************************************************************
* ��ʾ����
********************************************************************************/
void oled_42_show_num(uint8_t num, uint8_t mode, uint8_t index) {
    if(num > 9) return ;
    if(mode == 0)
        oled_42_show_pic_index(0,0,72,40, number_array_1_0[num],1, index);
    else if(mode == 1)
        oled_42_show_pic_index(0,0,72,40, number_array_1_1[num],1, index);
    else if(mode == 2)
        oled_42_show_pic_index(0,0,72,40, number_array_2_0[num],1, index);
    else
        oled_42_show_pic_index(0,0,72,40, number_array_2_1[num],1, index);
}
