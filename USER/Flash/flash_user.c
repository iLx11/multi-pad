/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/26 14:27
* @version: 1.0
* @description:
********************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "flash_user.h"
#include "flash.h"

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
* 擦除或重置菜单数据
********************************************************************************/
void reset_menu_data(uint8_t index, uint8_t func) {
    if(func == 0) {
        uint8_t temp = index << PER_FOLDER_SECTOR_SHIFT;
        // 键值重置
        erase_flash_sector(temp);
        erase_flash_sector(temp + 1);
    } else if(func == 1) {
        // 单色图片重置
        uint8_t temp[720] = {0};
        storage_menu_to_flash(index, (uint8_t *) temp, 720, 1);
    } else if(func == 2) {
        uint32_t sector = (index * 27) + 22;
        // 彩色图片重置
        for(uint8_t i = 0; i < 27; i ++) {
            erase_flash_sector(sector + i);
        }
    }
}

/********************************************************************************
* 按照菜单写入键值和单色屏，mode == 1 时为读取屏幕数据
********************************************************************************/
void storage_menu_to_flash(uint8_t menu_index, uint8_t * data_buff, uint16_t data_size, uint8_t mode) {
    if(menu_index > 10) return ;
    uint32_t sector_address = menu_index << PER_FOLDER_SECTOR_SHIFT << 12;
    // 读取每一层的小屏幕 4096 + 3072
    if(mode == 1) sector_address += 7168;
    enable_flash_write();
    write_to_flash(data_buff, sector_address, data_size);
}

/********************************************************************************
* 键值的第二页
********************************************************************************/
void storage_page_two(uint8_t menu_index, uint8_t * data_buff, uint16_t data_size) {
    if(menu_index > 10) return ;
    uint32_t sector_address = menu_index << PER_FOLDER_SECTOR_SHIFT << 12;
    sector_address += 4096;
    enable_flash_write();
    write_to_flash(data_buff, sector_address, data_size);
}

/********************************************************************************
* 按照菜单读取键值和单色屏
********************************************************************************/
void load_menu_from_flash(uint8_t menu_index, uint8_t * data_buff, uint16_t data_size, uint8_t mode) {
    if(menu_index > 10) return ;
    uint32_t sector_address = menu_index << PER_FOLDER_SECTOR_SHIFT << 12;
    if(mode == 1) sector_address += 7168;
    read_flash(data_buff, sector_address, data_size);
}

/********************************************************************************
* 写入彩色屏幕，每层占据 27 个扇区，十层菜单占据 16 个块
********************************************************************************/
void storage_color_screen(uint8_t menu_index, uint32_t address_offset, uint8_t * data_buff, uint16_t data_size) {
    if(menu_index > 10) return ;
    uint32_t sector_address = ((menu_index * 27) + 22) << 12;
    sector_address += address_offset;
    enable_flash_write();
    write_to_flash(data_buff, sector_address, data_size);
}

/********************************************************************************
* 读取彩色屏幕，每层占据 27 个扇区，十层菜单占据 16 个块
********************************************************************************/
void read_color_screen(uint8_t menu_index, uint32_t address_offset, uint8_t * data_buff, uint16_t data_size) {
    if(menu_index > 10) return ;
    uint32_t sector_address = ((menu_index * 27) + 22) << 12;
    sector_address += address_offset;
    read_flash(data_buff, sector_address, data_size);
}

/********************************************************************************
* 加载菜单配置
********************************************************************************/
void load_config(uint8_t * data_buff, uint16_t data_size) {
    read_flash(data_buff, 2048, data_size);
}

/********************************************************************************
* 存储配置
********************************************************************************/
void storage_config(uint8_t * data_buff, uint16_t data_size) {
    enable_flash_write();
    write_to_flash(data_buff, 2048, data_size);
}

