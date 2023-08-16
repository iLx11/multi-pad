#include "fatfs_user.h"
#include "stm32f1xx.h"
#include <stdio.h>

uint8_t TEXT_Buffer[4096];
// 写入缓存数组
const uint8_t file_write_buff[] = "hello world!!!!";
// 读取缓存数组
uint8_t file_read_buff[4096] = {0};
FATFS fsObject;//每个设备挂载都需要一个
FIL Fil;
// 结果变量
FRESULT res;
// 字节读取与字节写入
UINT bw, br;
FATFS **fs;
DWORD freeClusters;

// 文件系统挂载
static void file_sys_mount();

// 文件系统取消挂载
static void file_sys_unmount();

// 文件写入
static FRESULT file_sys_write();

// 文件读取
static FRESULT file_sys_read();

void file_sys_init() {
    // SPI
    SPI2_Init();
    // flash
    norflash_init();
    // 读取 flash id
    printf("%d", norflash_read_id());
    // 挂载
    file_sys_mount();

    res = f_open(&Fil, "0:1.bin", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);      //打开/创建文件
    printf("f_mkdir=%d\n", res);
    if (res == FR_OK) {
        // 读取
        if (file_sys_write() == FR_OK) {
            if (file_sys_read() == FR_OK) {
                printf("\n内容：%s", file_read_buff);
            }
        }
        f_close(&Fil);
    }
}

// 文件系统挂载
static void file_sys_mount() {
    // 挂载文件系统
    res = f_mount(&fsObject, "0:", 1);
    if (res != 0) {
        printf("f_mount=%d\n", res);
        res = f_mkfs("0:", 0, TEXT_Buffer, 4096);
        printf("f_mkfs=%d\n", res);//初始化以后要重新挂载
        file_sys_unmount();
    }
}

// 文件系统取消挂载
static void file_sys_unmount() {
    f_mount(NULL, "0:", 1);                                    //取消挂载
    f_mount(&fsObject, "0:", 1);
}

// 文件写入
static FRESULT file_sys_write() {
    res = f_write(&Fil, file_write_buff, sizeof(file_write_buff), &bw);        //写内容
    printf("f_write=%d bW=%d\n", res, bw);
    return res;
}

// 文件读取
static FRESULT file_sys_read() {
    /* 移动打开的文件对象的文件读/写指针到 0
     * ofs: 距文件顶部的字节偏移量
     */
    f_lseek(&Fil, 0);
    /* 读取文件
     * btr: UINT 类型范围内要读取的字节数
     */
    res = f_read(&Fil, file_read_buff, f_size(&Fil), &br);
    printf("f_read=%d bW=%d\n", res, br);
    return res;
}

