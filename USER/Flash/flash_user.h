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

// ��� FLASH SIZE -> 16M (128 / 8)
#define MAX_FLASH_SIZE (16 * 1024 * 1024);
// ������ (16M / 4096 / 16)
#define BLOCK_NUM 256
// ������С
#define SECTOR_SIZE 4096
// ÿһ��Ĵ洢ƫ�ƣ�* 4��
#define PER_FOLDER_SECTOR_SHIFT 1


uint8_t flash_init_user(void);

void storage_menu_to_flash(uint8_t menu_index, uint8_t * data_buff, uint16_t data_size, uint8_t mode);

void load_menu_from_flash(uint8_t menu_index, uint8_t * data_buff, uint16_t data_size, uint8_t mode);

#endif //MULTI_FLASH_USER_H
