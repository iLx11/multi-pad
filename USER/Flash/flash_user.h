/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/26 14:27
* @version: 1.0
* @description: 
********************************************************************************/


#ifndef MULTI_FLASH_USER_H
#define MULTI_FLASH_USER_H

#include "stm32f1xx_hal.h"

// 最大 FLASH SIZE -> 16M (128 / 8)
#define MAX_FLASH_SIZE (16 * 1024 * 1024);
// 块数量 (16M / 4096 / 16)
#define BLOCK_NUM 256
// 扇区大小
#define SECTOR_SIZE 4096
// 每一层的存储偏移（* 4）
#define PER_FOLDER_SECTOR_SHIFT 1

uint8_t flash_init_user(void);

void reset_menu_data(uint8_t index, uint8_t func);

void storage_menu_to_flash(uint8_t menu_index, uint8_t * data_buff, uint16_t data_size, uint8_t mode);

void load_menu_from_flash(uint8_t menu_index, uint8_t * data_buff, uint16_t data_size, uint8_t mode);

void load_config(uint8_t * data_buff, uint16_t data_size);

void storage_config(uint8_t * data_buff, uint16_t data_size);

void storage_page_two(uint8_t menu_index, uint8_t * data_buff, uint16_t data_size);

void storage_color_screen(uint8_t menu_index, uint32_t address_offset, uint8_t * data_buff, uint16_t data_size);

void read_color_screen(uint8_t menu_index, uint32_t address_offset, uint8_t * data_buff, uint16_t data_size);

#endif //MULTI_FLASH_USER_H
