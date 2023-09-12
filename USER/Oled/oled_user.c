#include "oled_user.h"
#include "oled.h"
#include "flash_user.h"
#include "encoder_user.h"

extern uint8_t menu_change_lock;

void load_menu_from_flash(uint8_t folderIndex);

void OLED_ShowPicture(uint8_t menu_index);

// 图片存储数组
uint8_t oled_42_array[OLED_42_NUM][SIZE_42];
uint8_t oled_96_array[OLED_96_NUM][SIZE_96];

// 乱序数组
uint8_t random_refresh[3][20] = {7, 14, 17, 10, 11, 3, 18, 5, 13, 15, 9, 1, 0, 2, 6, 8, 16, 4, 19, 12,
                                 9, 12, 6, 8, 4, 19, 0, 7, 17, 3, 13, 15, 1, 16, 10, 11, 2, 14, 5, 18,
                                 3, 18, 15, 13, 16, 10, 4, 1, 0, 14, 7, 17, 11, 5, 9, 19, 12, 6, 2, 8};

// 初始化
void oled_init_user() {
    OLED_42_Init();
    OLED_92_Init();
//    OLED_42_ShowPicture(0,0,72,42,BMP0,1,1);
    load_show_menu(0);
}

/********************************************************************************
* 菜单读取后显示
********************************************************************************/
void load_show_menu(uint8_t menu_index) {
    menu_change_lock = 1;
    load_menu_from_flash(menu_index);
    OLED_ShowPicture(menu_index);
    menu_change_lock = 0;
}

/********************************************************************************
* 从 flash 加载图片
********************************************************************************/
void load_menu_from_flash(uint8_t folderIndex) {
    for (int i = 0; i < 20; i++) {
         ReadPhoto(folderIndex, i, oled_42_array[i], SIZE_42);
         if (i < OLED_96_NUM)
             ReadPhoto(folderIndex , i + OLED_42_NUM, oled_96_array[i], SIZE_96);
    }
}

/********************************************************************************
* 显示图片
********************************************************************************/
void OLED_ShowPicture(uint8_t menu_index) {
    for (int i = 0; i < OLED_42_NUM; i++) {
        OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                            oled_42_array[random_refresh[menu_index % 3][i]], 1, random_refresh[menu_index % 3][i]);
        if (i < OLED_96_NUM)
            OLED_92_ShowPicture(oled_96_x, oled_96_y, oled_96_l, oled_96_h, oled_96_array[i], 1, i);
    }
}

// 图片写入 flash 存储
void menu_storage(void) {
//    W25QXX_Init();
//    for(int i=0; i<PIC_NUM; i++) {
//        if(i<20)
//        {
//            StorePhoto(1,i,bmp_buffer2[i],SIZE_42);
//        }
//        else
//        {
//            StorePhoto(1,i,bmp_buffer2[i],SIZE_96);
//        }
//    }
    //StorePhoto(0,12,bmp_buffer[12],SIZE);
    // StorePhoto(0,1,BMP2,SIZE2);
    // load_meun(0);
    // ReadPhoto(0,1,buffer2,SIZE);
}


