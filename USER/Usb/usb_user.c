#include "usb_user.h"
#include "oled_user.h"
#include "flash_user.h"
#include "usbd_customhid.h"
#include "jsmn_user.h"
#include "usbd_cdc_if.h"
#include "lcd_user.h"
#include "oled_42.h"

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
//extern char json_str[JSON_SIZE];

#define RE_BUFF_SIZE 4096

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
uint16_t package_size;
// 数据状态
uint8_t data_state;
// 彩屏数据模式
uint8_t color_mode;
// 彩屏数据包计算
uint32_t color_package_count = 0;
// 等待信号
uint8_t sign[1] = {0x68};
// 菜单配置数组
uint8_t menu_config_arr[31] = {0};

/********************************************************************************
* 开机配置
********************************************************************************/
void first_load_menu(void) {
    // 加载菜单配置
    load_menu_config();
    load_menu(menu_config_arr[0]);
}

/********************************************************************************
* 加载菜单
********************************************************************************/
void load_menu(uint8_t index) {
    if(index < 1 || index > 10) return;
    load_parse_key(index);
    show_menu_oled(index);
    show_menu_color(index);
}

/********************************************************************************
* USB 扫描
********************************************************************************/
void usb_scan_user(void) {
    // 填充满接收缓冲数组之后写入 FLASH
    if (data_receive_flag == 0xff) {
        data_receive_flag &= 0x00;
        // 写入菜单
        if (menu_config == 0xff) {
            menu_config &= 0x00;
            storage_menu_to_flash(0, (uint8_t *) &receive_buff, package_size, 0);
            receive_reset();
            CDC_Transmit_FS(sign, 1);
            return;
        }
        // 写入键值与图片
        // 单层图片
        if (data_state_flag == 0xff) {
            if (package_size > 730) return;
            data_state_flag &= 0x00;
            // 转为图片数据后存储在 FLASH
            storage_menu_to_flash(write_menu, (uint8_t *) receive_buff, package_size, 1);
            // ------------- test --------------
//            show_menu_oled(write_menu);
            data_state &= 0x00;
            receive_reset();
            CDC_Transmit_FS(sign, 1);
            return;
        }
        // 彩屏图片
        if (color_mode == 0xff) {
            // 存储图片
            storage_color_screen(write_menu, color_package_count << 12, receive_buff, package_size);
            // 图片传输完成判断
            if (color_package_count < 26) {
                color_package_count++;
            } else {
                // 图片传输完成
                color_package_count = 0;
                color_mode &= 0x00;
                // 切换菜单
                write_menu ++;
                turn_next_menu();
                // --------------- test -----------------
//                load_menu(write_menu);
            }
            receive_reset();
            CDC_Transmit_FS(sign, 1);
            return;
        }
        // 写入键值
        if (data_state == 0xff) {
            // 键值存储的第二个状态
            data_state &= 0x00;
            storage_page_two(write_menu, (uint8_t *) &receive_buff, 3072);
        } else {
            // 键值存储的第一个状态
            data_state |= 0xff;
            storage_menu_to_flash(write_menu, (uint8_t *) &receive_buff, 4096, 0);
        }
        receive_reset();
        CDC_Transmit_FS(sign, 1);

    }
}

/********************************************************************************
* 加载菜单配置
********************************************************************************/
void load_menu_config(void) {
    load_menu_from_flash(0, menu_config_arr, 31, 0);
}

/********************************************************************************
* 跳转到有内容的下一级菜单
********************************************************************************/
void turn_next_menu(void) {
    while(menu_config_arr[write_menu] == 0x00) {
        if(write_menu > 10) {
            write_menu = 1;
            return;
        }
        write_menu += 1;
    }
}

/********************************************************************************
* 获取键值单页键值的长度
********************************************************************************/
uint32_t get_key_size(uint8_t index) {
    uint8_t p = (index * 2) + 10;
    return menu_config_arr[p - 1] * 255 + menu_config_arr[p];
}

/********************************************************************************
* 重置标志
********************************************************************************/
static void receive_reset(void) {
    package_size = 0;
    // 清空接收缓存数组
    memset(receive_buff, 0, RE_BUFF_SIZE);
}

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
void hid_buff_reset(void) {
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
