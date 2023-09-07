#ifndef LEDEN_FLASH_H
#define LEDEN_FLASH_H

#include "norflash.h"
#include <stdio.h>
#include "bmp.h"

void StorePhoto(uint8_t folderIndex, uint8_t photoIndexInFolder, uint8_t *photoData, uint16_t photoSize);
void ReadPhoto(uint8_t folderIndex, uint8_t photoIndex, uint8_t *photoData,uint16_t photoSize);
void flash_init_user();



#endif //LEDEN_FLASH_H
