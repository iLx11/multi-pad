#include "usb_user.h"

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
// 当前的 buff
uint8_t cur_buff = 0;

buff_struct buff_point_array[3] = {
        {.send_buff_point = send_key_buff, .zero_buff_point = send_key_zero_buff, .buff_size = 9},
        {.send_buff_point = send_mouse_buff, .zero_buff_point = send_mouse_zero_buff, .buff_size = 5},
        {.send_buff_point = send_media_buff, .zero_buff_point = send_media_zero_buff, .buff_size = 5}
};

// 图片数组
uint8_t photo_42_array[20][360] = {0x00};
uint8_t photo_96_array[5][1024] = {0x00};


/********************************************************************************
* 接收 hid 发送的数据
********************************************************************************/
void receive_data_from_upper(USBD_CUSTOM_HID_HandleTypeDef *hid_handle, uint8_t len) {
    char hex_string_array[300];
    printf("\n\rString -> \n\r");
    for (uint8_t i = 0; i < len; i++) {
        printf("%c", hid_handle->Report_buf[i]);
        sprintf(hex_string_array + (i * 2), "%02x", hid_handle->Report_buf[i]);
    }
    printf("\r\n-------------------------------------------------------------------\r\n");
    printf("\n\rHex -> \n\r");
    printf("%s", hex_string_array);
    for (uint8_t i = 0; i < 200; i++) {
        printf("sdfs");
    }
}

// 字符转十六进制
static uint8_t string_to_num_hex(const char *hex_string_array, uint8_t start, uint8_t end) {
    uint8_t result = 0x00;
    // 小写
    if (hex_string_array[start] >= '0' && hex_string_array[start] <= '9')
        result += hex_string_array[start] - 0x30 * 16;
    else if (hex_string_array[start] >= 'a' && hex_string_array[start] <= 'f')
        result += (0x0A + hex_string_array[start] - 0x61) * 16;

    if (hex_string_array[end] >= '0' && hex_string_array[end] <= '9') {
        result += hex_string_array[start] - 0x30;
        return result;
    } else if (hex_string_array[end] >= 'a' && hex_string_array[end] <= 'f') {
        result += (0x0A + hex_string_array[end] - 0x61);
        return result;
    }
    // 大写
    if (hex_string_array[start] >= '0' && hex_string_array[start] <= '9')
        result += hex_string_array[start] - 0x30 * 16;
    else if (hex_string_array[start] >= 'A' && hex_string_array[start] <= 'F') {
        result += (0x0A + hex_string_array[start] - 0x41) * 16;
    }
    if (hex_string_array[start] >= '0' && hex_string_array[start] <= '9')
        result += hex_string_array[start] - 0x30 * 16;
    else if (hex_string_array[start] >= 'A' && hex_string_array[start] <= 'F') {
        result += (0x0A + hex_string_array[start] - 0x41);
    }
    return result;
}

/********************************************************************************
* 发送 hid 码
********************************************************************************/
void send_hid_code(uint8_t func) {
    cur_buff = func;
    while (USBD_CUSTOM_HID_FUNC_SendReport(&hUsbDeviceFS, buff_point_array[func].send_buff_point,
                                           buff_point_array[func].buff_size) != USBD_OK);
}

/********************************************************************************
* 发送 hid 重置
********************************************************************************/
void hid_buff_reset() {
    for (uint8_t i = 0; i < buff_point_array[cur_buff].buff_size; i++) {
        buff_point_array[cur_buff].send_buff_point[i] &= 0x00;
    }
    while (USBD_CUSTOM_HID_FUNC_SendReport(&hUsbDeviceFS, buff_point_array[cur_buff].zero_buff_point,
                                           buff_point_array[cur_buff].buff_size) != USBD_OK);
    printf("reset ok\n");
}
