#include <malloc.h>
#include "oled_user.h"
#include "flash_user.h"
#include "oled_42.h"
#include "number_pic.h"

// ��ʼ��
void oled_init_user() {
    oled_42_init();
}

/********************************************************************************
* ��ʾ��ǰ�㼶��ͼƬ
********************************************************************************/
void show_menu_oled(uint8_t menu_index) {
    // ��ȡͼƬ
    uint8_t* tempArr = (uint8_t *) malloc(sizeof (uint8_t) * 720);
    load_menu_from_flash(menu_index, tempArr, 720, 1);
    oled_42_show_pic_index(0, 0, 72, 40, tempArr,  1, 0);
    oled_42_show_pic_index(0, 0, 72, 40, tempArr + 360,  1, 1);
    free(tempArr);
    tempArr = NULL;
}

/********************************************************************************
* ��ʾ����
********************************************************************************/
void oled_42_show_num(uint8_t num, uint8_t mode, uint8_t index) {
    if(num > 9) return ;
//    if(mode)
//        oled_42_show_pic_index(0,0,72,40, number_array_2_1[num],1, index);
//    else
    oled_42_show_pic_index(0,0,72,40, number_array_2_0[num],1, index);
}
