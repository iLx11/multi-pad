#include "key_user.h"


extern USBD_HandleTypeDef hUsbDeviceFS;

// hid 发送
// 键盘
uint8_t send_key_buff[9] = {0};
uint8_t send_key_zero_buff[9] = {0x01, 0, 0, 0, 0, 0, 0, 0, 0};
// 鼠标
uint8_t send_mouse_buff[5] = {0};
uint8_t send_mouse_zero_buff[5] = {0x02, 0, 0, 0, 0};
// 媒体
uint8_t send_media_buff[5] = {0};
uint8_t send_media_zero_buff[5] = {0x03, 0x00, 0x00, 0x00, 0x00};
uint8_t mouse_step = 30;
// 当前的 buff
uint8_t cur_buff = 0;

buff_struct buff_point_array[3] = {
        {.send_buff_point = send_key_buff, .zero_buff_point = send_key_zero_buff, .buff_size = 9},
        {.send_buff_point = send_mouse_buff, .zero_buff_point = send_mouse_zero_buff, .buff_size = 5},
        {.send_buff_point = send_media_buff, .zero_buff_point = send_media_zero_buff, .buff_size = 5}
};
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
    hid_buff_reset();
    HAL_Delay(10);
}

void MK_on_keydown(uint8_t row, uint8_t col) {
    uint8_t key_value = (col * 4) + row;
    printf("key_value -> %d\n", key_value);
    parse_json_value(key_value);
    send_media_buff[0] = 0x03;
    /*switch (key_value) {
        case 3: {
            buff_point_array[2].send_buff_point[1] = 0x01;
            break;
        }
        case 7: {
            buff_point_array[2].send_buff_point[1] = 0x02;
            break;
        }
        case 11: {
            buff_point_array[2].send_buff_point[1] = 0x04;
            break;
        }
        default:
            break;
    }
    send_hid_code(2);*/
}

// 发送 hid 码
void send_hid_code(uint8_t func) {
    cur_buff = func;
    while (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, buff_point_array[func].send_buff_point,
                                      buff_point_array[func].buff_size) != USBD_OK);
}

// 发送 hid 重置
void hid_buff_reset() {
    for (uint8_t i = 0; i < buff_point_array[cur_buff].buff_size; i++) {
        buff_point_array[cur_buff].send_buff_point[i] &= 0x00;
    }
    while (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, buff_point_array[cur_buff].zero_buff_point,
                                      buff_point_array[cur_buff].buff_size) != USBD_OK);
    printf("reset ok\n");
}


