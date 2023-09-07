#include "oled_user.h"
#include "oled.h"
#include "norflash.h"
#include "flash_user.h"


u8 buffer0[SIZE_42];
u8 buffer1[SIZE_42];
u8 buffer2[SIZE_42];
u8 buffer3[SIZE_42];
u8 buffer4[SIZE_42];
u8 buffer5[SIZE_42];
u8 buffer6[SIZE_42];
u8 buffer7[SIZE_42];
u8 buffer8[SIZE_42];
u8 buffer9[SIZE_42];
u8 buffer10[SIZE_42];
u8 buffer11[SIZE_42];
u8 buffer12[SIZE_42];
u8 buffer13[SIZE_42];
u8 buffer14[SIZE_42];
u8 buffer15[SIZE_42];
u8 buffer16[SIZE_42];
u8 buffer17[SIZE_42];
u8 buffer18[SIZE_42];
u8 buffer19[SIZE_42];
u8 buffer20[SIZE_96];
u8 buffer21[SIZE_96];
u8 buffer22[SIZE_96];
u8 buffer23[SIZE_96];
u8 buffer24[SIZE_96];

u8* pbuffer[PIC_NUM]=
        {
                buffer0,buffer1,buffer2,buffer3,buffer4,buffer5,buffer6,buffer7,
                buffer8,buffer9,buffer10,buffer11,buffer12,buffer13,buffer14,
                buffer15,buffer16,buffer17,buffer18,buffer19,buffer20,buffer21,
                buffer22,buffer23,buffer24
        };

void load_meun(uint8_t folderIndex) {
    for(int i=0; i<PIC_NUM; i++) {
        if(i<20)
        {
            ReadPhoto(folderIndex,i,pbuffer[i],SIZE_42);
        }
        else
        {
            ReadPhoto(folderIndex,i,pbuffer[i],SIZE_96);
        }

    }



}
void Menu_init(void) {
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
void OLED_ShowPicture(){
    for(int i=0; i<PIC_NUM; i++) {
        if(i<20)
        {
            OLED_42_ShowPicture(oled_42_x,oled_42_y,oled_42_l,oled_42_h,pbuffer[i],1,i);
        }
        else
        {
            OLED_92_ShowPicture(oled_96_x,oled_96_y,oled_96_l,oled_96_h,pbuffer[i],1,i-20);
        }
    }

}
void Menu1(void) {
    load_meun(0);
    OLED_ShowPicture();

}
void Menu2(void) {
    load_meun(1);
    OLED_ShowPicture();

}

void oled_init_user() {
    OLED_42_Init();
//    OLED_92_Init();
//    OLED_42_ShowPicture(0,0,72,42,BMP0,1,1);
    printf("OLED_42_Init -> %d\n");
    Menu1();
}
//#include "w25qxx.h"
