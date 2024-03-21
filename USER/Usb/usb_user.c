#include "usb_user.h"
#include "oled_user.h"
#include "flash_user.h"
#include "usbd_customhid.h"
#include "jsmn_user.h"
#include "usbd_cdc_if.h"

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

// 键盘命令
extern char json_str[JSON_SIZE];
//char clear_str[JSON_SIZE] = {0};

#define RE_BUFF_SIZE (4096)

// 菜单索引
uint8_t write_menu = 1;
// 接收 FLAG
uint8_t data_receive_flag;
// 数据状态
uint8_t data_state_flag;
// 菜单配置
uint8_t menu_config;
// 接收缓存数组
uint8_t receive_buff[RE_BUFF_SIZE];
// 数据包的大小
uint16_t package_size = 0;
// 数据状态
uint8_t data_state = 0;

/********************************************************************************
* USB 扫描
********************************************************************************/
void usb_scan_user() {
    // 填充满接收缓冲数组之后写入 FLASH
    // 写入菜单
    if(menu_config == 0xff) {
        menu_config ^= 0x00;
        storage_menu_to_flash(0, (uint8_t *)&receive_buff, package_size, 0);
        package_size = 0;
        return;
    }
    // 写入键值与图片
    if(data_receive_flag == 0xff) {
        data_receive_flag ^= 0x00;
        // 单层图片
        if(data_state_flag == 0xff) {
            if(package_size > 700) return;
            data_state_flag ^= 0x00;
            // 存储小屏幕图片
            storage_menu_to_flash(write_menu, (uint8_t *)&receive_buff, package_size, 1);
            return;
        }
        // 写入键值
        if(data_state == 1) {
            // 键值存储的第二个状态
            data_state = 0;
            storage_menu_to_flash(write_menu, (uint8_t *)&receive_buff, package_size + 4096, 0);
        } else {
            // 键值存储的第一个状态
            data_state = 1;
            storage_menu_to_flash(write_menu, (uint8_t *)&receive_buff, package_size, 0);
        }

        // ------------------ test --------------------------
//        memset(json_str, 0, JSON_SIZE);
//        memcpy(json_str, receive_buff, package_size);
//        load_parse_key(1);
        // --------------------------------------------------
        // 清空接收缓存数组
//        memset(receive_buff, 0, RE_BUFF_SIZE);
        package_size = 0;
    }
}

//for (uint8_t i = 0; i < len; i++) {
//sprintf(hex_string_array + (i * 2), "%02x", hid_handle->Report_buf[i]);
//}

/********************************************************************************
* 发送 hid 码
********************************************************************************/
void send_hid_code(uint8_t func) {
    cur_buff = func;
    while (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, buff_point_array[func].send_buff_point,
                                           buff_point_array[func].buff_size) != USBD_OK);
}

/********************************************************************************
* 发送 hid 重置
********************************************************************************/
void hid_buff_reset() {
    for (uint8_t i = 0; i < buff_point_array[cur_buff].buff_size; i++) {
        buff_point_array[cur_buff].send_buff_point[i] &= 0x00;
    }
    while (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, buff_point_array[cur_buff].zero_buff_point,
                                           buff_point_array[cur_buff].buff_size) != USBD_OK);
    printf("reset ok\n");
}

/********************************************************************************
* 字符串转十六进制
********************************************************************************/
static uint8_t string_to_num(const char *hex_string_array, uint8_t start, uint8_t end) {
    return char_to_hex(hex_string_array[start]) * 16 + char_to_hex(hex_string_array[end]);
}

/********************************************************************************
* 字符转十六进制
********************************************************************************/
static uint8_t char_to_hex(char hex_char) {
    uint8_t result = 0x00;
    if (hex_char >= '0' && hex_char <= '9')
        result += (hex_char - 0x30);
    else if (hex_char >= 'a' && hex_char <= 'f')
        result += (0x0A + hex_char - 0x61);
    else if (hex_char >= 'A' && hex_char <= 'F') {
        result += (0x0A + hex_char - 0x41);
    }
    return result;
}
