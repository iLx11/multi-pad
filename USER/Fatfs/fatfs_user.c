#include "fatfs_user.h"
#include "stm32f1xx.h"
#include <stdio.h>

uint8_t TEXT_Buffer[4096];
// д�뻺������
const uint8_t file_write_buff[] = "hello world!!!!";
// ��ȡ��������
uint8_t file_read_buff[4096] = {0};
FATFS fsObject;//ÿ���豸���ض���Ҫһ��
FIL Fil;
// �������
FRESULT res;
// �ֽڶ�ȡ���ֽ�д��
UINT bw, br;
FATFS **fs;
DWORD freeClusters;

// �ļ�ϵͳ����
static void file_sys_mount();

// �ļ�ϵͳȡ������
static void file_sys_unmount();

// �ļ�д��
static FRESULT file_sys_write();

// �ļ���ȡ
static FRESULT file_sys_read();

void file_sys_init() {
    // SPI
    SPI2_Init();
    // flash
    norflash_init();
    // ��ȡ flash id
    printf("%d", norflash_read_id());
    // ����
    file_sys_mount();

    res = f_open(&Fil, "0:1.bin", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);      //��/�����ļ�
    printf("f_mkdir=%d\n", res);
    if (res == FR_OK) {
        // ��ȡ
        if (file_sys_write() == FR_OK) {
            if (file_sys_read() == FR_OK) {
                printf("\n���ݣ�%s", file_read_buff);
            }
        }
        f_close(&Fil);
    }
}

// �ļ�ϵͳ����
static void file_sys_mount() {
    // �����ļ�ϵͳ
    res = f_mount(&fsObject, "0:", 1);
    if (res != 0) {
        printf("f_mount=%d\n", res);
        res = f_mkfs("0:", 0, TEXT_Buffer, 4096);
        printf("f_mkfs=%d\n", res);//��ʼ���Ժ�Ҫ���¹���
        file_sys_unmount();
    }
}

// �ļ�ϵͳȡ������
static void file_sys_unmount() {
    f_mount(NULL, "0:", 1);                                    //ȡ������
    f_mount(&fsObject, "0:", 1);
}

// �ļ�д��
static FRESULT file_sys_write() {
    res = f_write(&Fil, file_write_buff, sizeof(file_write_buff), &bw);        //д����
    printf("f_write=%d bW=%d\n", res, bw);
    return res;
}

// �ļ���ȡ
static FRESULT file_sys_read() {
    /* �ƶ��򿪵��ļ�������ļ���/дָ�뵽 0
     * ofs: ���ļ��������ֽ�ƫ����
     */
    f_lseek(&Fil, 0);
    /* ��ȡ�ļ�
     * btr: UINT ���ͷ�Χ��Ҫ��ȡ���ֽ���
     */
    res = f_read(&Fil, file_read_buff, f_size(&Fil), &br);
    printf("f_read=%d bW=%d\n", res, br);
    return res;
}

