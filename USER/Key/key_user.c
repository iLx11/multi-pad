#include "key_user.h"


extern USBD_HandleTypeDef hUsbDeviceFS;

// hid ·¢ËÍ
// ¼üÅÌ
uint8_t send_key_buff[9] = {0};
uint8_t send_key_zero_buff[9] = {0x01, 0, 0, 0, 0, 0, 0, 0, 0};
// Êó±ê
uint8_t send_mouse_buff[5] = {0};
uint8_t send_mouse_zero_buff[5] = {0x02, 0, 0, 0, 0};

// Êó±ê
uint8_t send_media_buff[5] = {0};
uint8_t send_media_zero_buff[5] = {0x03, 0x00, 0x00, 0x00, 0x00};
uint8_t mouse_step = 30;
// ¼üÅÌ³õÊ¼×´Ì¬
static uint16_t state = 0xffff;


// ¼üÅÌ³õÊ¼»¯
void key_init_user() {
    MK_init();
}

// ¼üÅÌÖ´ÐÐÉ¨Ãè
void key_scan_user() {
    MK_scan(&state);
}

// °´¼üÖ´ÐÐ
void MK_on_keyup(uint8_t row, uint8_t col) {
    send_hid_code(0);
    printf("clear key");
    HAL_Delay(10);
}

void MK_on_keydown(uint8_t row, uint8_t col) {
    uint8_t key_value = (col * 4) + row;
    printf("key_value -> %d\n", key_value);
    send_media_buff[0] = 0x03;
    switch (key_value) {
        case 3: {
           send_media_buff[1] = 0x01;
            break;
        }
        case 7: {
            send_media_buff[1] = 0x02;
            break;
        }
        case 11: {
            send_media_buff[1] = 0x04;
            break;
        }
        default:
            break;
    }
/*    send_mouse_buff[0] = 0x02;
    uint8_t X = 0, Y = 0, SCROLL = 0;
    switch (key_value) {
        case 0:
            send_mouse_buff[1] = 0x01;
            break;
        case 4:
            send_mouse_buff[1] = 0x02;
            break;
        case 8:
            send_mouse_buff[1] = 0x04;
            break;
        case 2: {
            SCROLL += mouse_step;
            send_mouse_buff[4] = SCROLL;
            break;
        }
        case 6: {
            SCROLL -= mouse_step;
            send_mouse_buff[4] = SCROLL;
            break;
        }
        case 1: {
            X += mouse_step;
            send_mouse_buff[2] = X;
            break;
        }
        case 5: {
            Y += mouse_step;
            send_mouse_buff[3] = Y;
            break;
        }
        case 9: {
            Y -= mouse_step;
            send_mouse_buff[3] = Y;
            break;
        }
        case 13: {
            X -= mouse_step;
            send_mouse_buff[2] = X;
            break;
        }
        default:
            break;
    }*/
    send_hid_code(3);
}

// ·¢ËÍ hid Âë
void send_hid_code(uint8_t func) {
    if (func == 1) { while (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, send_key_buff, 9) != USBD_OK); }
    else if (func == 2) { while (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, send_mouse_buff, 5) != USBD_OK); }
    else if (func == 3) { while (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, send_media_buff, 5) != USBD_OK); }
    else if (func == 0) {
        /*for(uint8_t i = 0; i < sizeof (send_key_buff); i ++) {
            send_key_buff[i] &= 0x00;
        }
        for(uint8_t i = 0; i < sizeof (send_mouse_buff); i ++) {
            send_mouse_buff[i] &= 0x00;
        }*/
        for(uint8_t i = 0; i < sizeof (send_media_buff); i ++) {
            send_media_buff[i] &= 0x00;
        }
        /*while (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, send_key_zero_buff, 9) != USBD_OK);
        while (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, send_mouse_zero_buff, 5) != USBD_OK);*/
        while (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, send_media_zero_buff, 5) != USBD_OK);
    }
}


