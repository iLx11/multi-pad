#ifndef LEDEN_FLASH_H
#define LEDEN_FLASH_H

#include "norflash.h"
#include <stdio.h>
//#include "bmp.h"

void storage_to_flash(uint8_t folder_index, uint8_t photo_index, uint8_t *photo_data, uint16_t photo_size);

void read_from_flash(uint8_t folder_index, uint8_t photoIndex, uint8_t *photo_data,uint16_t photo_size);

void flash_init_user();

void menu_storage();


#endif //LEDEN_FLASH_H
