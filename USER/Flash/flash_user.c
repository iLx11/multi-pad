#include "flash_user.h"
#include "oled_user.h"
#include "usb_user.h"

#define NUM_FOLDERS 10
#define NUM_PHOTOS_PER_FOLDER 26
#define SECTOR_SIZE 4096


// 假设保存照片大小的数组
//uint16_t flash_folder[NUM_FOLDERS][NUM_PHOTOS_PER_FOLDER];

extern uint8_t oled_42_array[OLED_42_NUM][SIZE_42];
extern uint8_t oled_96_array[OLED_96_NUM][SIZE_96];

// 初始化
void flash_init_user() {
    norflash_init();
    printf("read_id -> %d\n", norflash_read_id());
}

/********************************************************************************
* 键值写入 flash 存储
********************************************************************************/
void storage_setting_to_flash(uint8_t folder_index, char* str_data, uint16_t str_size) {
    uint32_t sector = (folder_index * NUM_PHOTOS_PER_FOLDER + 25);
    norflash_write_enable(); // 解锁Flash
    norflash_write((uint8_t *)str_data, sector * SECTOR_SIZE, str_size);
//    norflash_write_disable();
}

/********************************************************************************
* 从 flash 读取键值
********************************************************************************/
void load_setting_from_flash(uint8_t folder_index, char* str_data, uint16_t str_size) {
    uint32_t sector = (folder_index * NUM_PHOTOS_PER_FOLDER + 25);
    norflash_read((uint8_t *)str_data, sector * SECTOR_SIZE, str_size);
}


/********************************************************************************
* 图片写入 flash 存储
********************************************************************************/
void menu_photo_folder_storage(uint8_t folder_index) {
    for (int i = 0; i < OLED_42_NUM; i++) {
            storage_to_flash(folder_index, i, oled_42_array[i], SIZE_42);
        if(i < OLED_96_NUM)
            storage_to_flash(folder_index, i + 20, oled_96_array[i], SIZE_96);
    }
}

/********************************************************************************
* 存储到 flash
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

