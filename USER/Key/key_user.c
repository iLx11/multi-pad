#include "key_user.h"


extern USBD_HandleTypeDef hUsbDeviceFS;

// hid ����
uint8_t send_buff[8] = {0};
uint8_t send_zero_buff[8] = {0, 0, 0, 0, 0, 0, 0, 0};


// ���̳�ʼ״̬
static uint16_t state = 0xffff;


// ���̳�ʼ��
void key_init_user() {
    MK_init();
}

// ����ִ��ɨ��
void key_scan_user() {
    MK_scan(&state);
}

// ����ִ��
void MK_on_keyup(uint8_t row, uint8_t col) {
    send_hid_code(0);
    printf("clear key");
    HAL_Delay(10);
}

void MK_on_keydown(uint8_t row, uint8_t col) {
    printf("��%d��,��%d�� ---down\n", row, col);
    uint8_t key_value = (col * 4) + row;
    parse_json_value(key_value);
    printf("send_buff -> %d", send_buff[2]);
    /*send_hid_code(1);
    printf("clear key");*/
}

// ���� hid ��
void send_hid_code(uint8_t func) {
    if (func == 1) while (USBD_HID_SendReport(&hUsbDeviceFS, send_buff, 8) != USBD_OK);
    else if (func == 0) while (USBD_HID_SendReport(&hUsbDeviceFS, send_zero_buff, 8) != USBD_OK);
}


