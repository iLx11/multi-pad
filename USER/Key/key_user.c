#include "key_user.h"


extern USBD_HandleTypeDef hUsbDeviceFS;

// hid 发送
uint8_t send_buff[8] = {0};
uint8_t send_zero_buff[8] = {0, 0, 0, 0, 0, 0, 0, 0};


// 键盘初始状态
static uint16_t state = 0xffff;


// 键盘初始化
void key_init_user() {
    MK_init();
}

// 键盘执行扫描
void key_scan_user() {
    MK_scan(&state);
}

// 按键执行
void MK_on_keyup(uint8_t row, uint8_t col) {
    while (USBD_HID_SendReport(&hUsbDeviceFS, send_zero_buff, 8) != USBD_OK);
    printf("clear key");
    HAL_Delay(10);
}



void MK_on_keydown(uint8_t row, uint8_t col) {
    printf("第%d行,第%d列 ---down\n", row, col);
    if (row + col == 0) {
        send_buff[0] = 0x01;
        send_buff[2] = 0x06;
        while (USBD_HID_SendReport(&hUsbDeviceFS, send_buff, 8) != USBD_OK);
        HAL_Delay(10);
        printf("ctrl + c\n");
    } else if (col - row == 1) {
        send_buff[0] = 0x01;
        send_buff[2] = 0x19;
        while (USBD_HID_SendReport(&hUsbDeviceFS, send_buff, 8) != USBD_OK);
        HAL_Delay(10);
        printf("ctrl + v\n");
    } else if (col - row == 2) {
        send_buff[0] = 0x01;
        send_buff[2] = 0x04;
        while (USBD_HID_SendReport(&hUsbDeviceFS, send_buff, 8) != USBD_OK);
        HAL_Delay(10);
        printf("ctrl + a\n");
    }
}


