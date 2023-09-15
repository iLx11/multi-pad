#include "flash_user.h"
#include "oled_user.h"
#include "usb_user.h"

#define NUM_FOLDERS 10
#define NUM_PHOTOS_PER_FOLDER 26
#define SECTOR_SIZE 4096


// 假设保存照片大小的数组
//uint16_t flash_folder[NUM_FOLDERS][NUM_PHOTOS_PER_FOLDER];

extern uint8_t photo_42_array[20][360];
extern uint8_t photo_96_array[5][1024];

// 初始化
void flash_init_user() {
    norflash_init();
    printf("read_id -> %d\n", norflash_read_id());
}

/********************************************************************************
* 图片写入 flash 存储
********************************************************************************/
void menu_storage() {
    for (int i = 0; i < OLED_42_NUM; i++) {
            storage_to_flash(1, i, photo_42_array[i], SIZE_42);
        if(i < OLED_96_NUM)
            storage_to_flash(1, i + 20, photo_96_array[i], SIZE_96);
    }
}

/********************************************************************************
* 存储图片
********************************************************************************/
void storage_to_flash(uint8_t folder_index, uint8_t photo_index, uint8_t *photo_data, uint16_t photo_size) {
    uint32_t sector = (folder_index * NUM_PHOTOS_PER_FOLDER + photo_index);
    norflash_write_enable(); // 解锁Flash
    norflash_write(photo_data, sector * SECTOR_SIZE, photo_size);
//    W25QXX_Write_Disable();
}

/********************************************************************************
* 读取图片数据
********************************************************************************/
void read_from_flash(uint8_t folder_index, uint8_t photoIndex, uint8_t *photo_data,uint16_t photo_size) {
    uint32_t sector = (folder_index * NUM_PHOTOS_PER_FOLDER + photoIndex);
    // 读取照片数据
    norflash_read(photo_data, sector * SECTOR_SIZE, photo_size);
}

