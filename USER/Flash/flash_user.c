#include "flash_user.h"


#define NUM_FOLDERS 10
#define NUM_PHOTOS_PER_FOLDER 26
#define SECTOR_SIZE 4096


// ���豣����Ƭ��С������
uint16_t photoSizes[NUM_FOLDERS][NUM_PHOTOS_PER_FOLDER];
void StorePhoto(uint8_t folderIndex, uint8_t photoIndexInFolder, uint8_t *photoData, uint16_t photoSize) {
    uint32_t sector = (folderIndex * NUM_PHOTOS_PER_FOLDER + photoIndexInFolder);
    norflash_write_enable(); // ����Flash
    norflash_write(photoData, sector * SECTOR_SIZE, photoSize);
//    W25QXX_Write_Disable();
}
void ReadPhoto(uint8_t folderIndex, uint8_t photoIndex, uint8_t *photoData,uint16_t photoSize) {
    uint32_t sector = (folderIndex * NUM_PHOTOS_PER_FOLDER + photoIndex);
    // ��ȡ��Ƭ����
    norflash_read(photoData, sector * SECTOR_SIZE, photoSize);
}
void flash_init_user() {
    norflash_init();
   printf("read_id -> %d\n", norflash_read_id());
}