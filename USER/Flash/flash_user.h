#ifndef LEDEN_FLASH_H
#define LEDEN_FLASH_H

#include "norflash.h"
#include <stdio.h>
//#include "bmp.h"

void storage_to_flash(uint8_t folder_index, uint8_t photo_index, uint8_t *photo_data, uint16_t photo_size);

void read_from_flash(uint8_t folder_index, uint8_t photoIndex, uint8_t *photo_data,uint16_t photo_size);

void flash_init_user();

void menu_photo_folder_storage(uint8_t folder_index);

void load_setting_from_flash(uint8_t folder_index, char* str_data, uint16_t str_size);

void storage_setting_to_flash(uint8_t folder_index, char* str_data, uint16_t str_size);


#endif //LEDEN_FLASH_H
