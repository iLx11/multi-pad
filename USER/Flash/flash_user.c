/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/26 14:27
* @version: 1.0
* @description: 
********************************************************************************/


#include <stdio.h>
#include "flash_user.h"
#include "flash.h"
#include "oled_user.h"

extern uint8_t oled_42_array[OLED_42_NUM][SIZE_42];

uint8_t flash_init_user(void) {
    flash_init();
    uint16_t flash_id = read_flash_id();
    if(flash_id == 0 || flash_id == 0xFFFF) {
        printf(" flash_read_id_error !\r\n");
        return 0;
    } else {
        printf("flash_id_read_done -> %d\r\n", flash_id);
    }
    return 1;
}

/********************************************************************************
* ���ղ˵�д���ֵ�͵�ɫ����mode == 1 ʱΪ��ȡ��Ļ����
********************************************************************************/
void storage_menu_to_flash(uint8_t menu_index, uint8_t * data_buff, uint16_t data_size, uint8_t mode) {
    if(menu_index > 10) return ;
    uint32_t sector_address = menu_index << PER_FOLDER_SECTOR_SHIFT << 12;
    // ��ȡÿһ���С��Ļ 4096 + 3072
    if(mode) sector_address += 7168;
    enable_flash_write();
    write_to_flash(data_buff, sector_address, data_size);
}

/********************************************************************************
* д���ɫ��Ļ��ÿ��ռ�� 27 ��������ʮ��˵�ռ�� 16 ����
********************************************************************************/
void storage_color_screen(uint8_t menu_index, uint8_t * data_buff, uint16_t data_size) {
    if(menu_index > 10) return ;
    menu_index += 11;
    uint32_t sector_address = menu_index << 12;
    enable_flash_write();
    write_to_flash(data_buff, sector_address, data_size);
}

/********************************************************************************
* ���ղ˵���ȡ��ֵ�͵�ɫ��
********************************************************************************/
void load_menu_from_flash(uint8_t menu_index, uint8_t * data_buff, uint16_t data_size, uint8_t mode) {
    if(menu_index > 10) return ;
    uint32_t sector_address = menu_index << PER_FOLDER_SECTOR_SHIFT << 12;
    if(mode) sector_address += 7168;
    read_flash(data_buff, sector_address, data_size);
}


