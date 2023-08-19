
#include "ff.h"      /* Obtains integer types */
#include "diskio.h"
#include "norflash.h"
/* Definitions of physical drive number for each drive */
#define DEV_FLASH    0  /* Example: Map Ramdisk to physical drive 0 */
uint16_t FLASH_SECTOR_COUNT = 2048 * 12;//前12M字节给FATFS占用
#define FLASH_SECTOR_SIZE 512
#define FLASH_BOLCK_SIZE

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
        BYTE pdrv    /* Physical drive nmuber to identify the drive */
) {
    DSTATUS stat;
    if (norflash_read_id() == 0xef17) {
        stat = 0;
    } else stat = STA_NOINIT;

    return stat;
}

//}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
        BYTE pdrv        /* Physical drive nmuber to identify the drive */
) {
    DSTATUS stat;
    /*W25QXX_WAKEUP();
    W25QXX_Init();*/

    norflash_wakeup();
    norflash_init();

    // DSTATUS stat;
    stat = disk_status(DEV_FLASH);
    return stat;
}
/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
        BYTE pdrv,    /* Physical drive nmuber to identify the drive */
        BYTE *buff,    /* Data buffer to store read data */
        LBA_t sector,  /* Start sector in LBA */
        UINT count    /* Number of sectors to read */
) {
    DRESULT res;
    // sector += 1536;
    /* FatFs文件系统是以扇区读写内容，我们实现的Flash读写是以字节读取，因此这里将扇区转换为字节位置，数量转换为字节 */
    norflash_read(buff, sector * 4096, count * 4096);
    res = RES_OK;
    return res;
}
/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
        BYTE pdrv,      /* Physical drive nmuber to identify the drive */
        const BYTE *buff,  /* Data to be written */
        LBA_t sector,    /* Start sector in LBA */
        UINT count      /* Number of sectors to write */
) {
    DRESULT res;
    norflash_write((uint8_t *) buff, sector * 4096, count * 4096);
    res = RES_OK;
    return res;
}

#endif
/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(//必须提前那格式化
        BYTE pdrv,    /* Physical drive nmuber (0..) */
        BYTE cmd,    /* Control code */
        void *buff    /* Buffer to send/receive control data */
) {
    DRESULT res;
    // Process of the command for the RAM drive
    switch (cmd) {
        //返回扇区个数
        case GET_SECTOR_COUNT:
            *(DWORD *) buff = 2048;
            //返回每个扇区大小
            break;
        case GET_SECTOR_SIZE:
            *(WORD *) buff = 4096;
            break;
            //返回擦除扇区的最小个数(单位为扇区)
        case GET_BLOCK_SIZE:
            *(WORD *) buff = 1;
            break;
    }
    res = RES_OK;
    return res;
}

DWORD get_fattime() {

    return 0;
}


//}

