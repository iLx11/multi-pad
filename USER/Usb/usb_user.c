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
uint16_t package_size;
// 数据状态
uint8_t data_state;
// 彩屏数据模式
uint8_t color_mode;
// 彩屏数据包计算
uint8_t color_package_mode = 1;

void usb_init_user(void) {
    /*uint8_t str[] = "abbaabbaabbaabbabaab";
    uint8_t* temp_arr = (uint8_t *) malloc(sizeof (uint8_t) * 30);
    for (uint8_t i = 0; i < 20 - 1; i++) {
        temp_arr[i] = string_to_num(str, i * 2, i * 2 + 1);
        printf("temp_arr -> %x\r\n", temp_arr[i]);
    }
    free(temp_arr);
    temp_arr = NULL;*/
}

/********************************************************************************
* USB 扫描
********************************************************************************/
void usb_scan_user(void) {
    // 填充满接收缓冲数组之后写入 FLASH
    if(data_receive_flag == 0xff) {
        data_receive_flag &= 0x00;
        // 写入菜单
        if(menu_config == 0xff) {
            menu_config &= 0x00;
            storage_menu_to_flash(0, (uint8_t *)&receive_buff, package_size, 0);
            package_size = 0;
            memset(receive_buff, 0, RE_BUFF_SIZE);
            return;
        }
        // 写入键值与图片
        // 单层图片
        if(data_state_flag == 0xff) {
            if(package_size > 700) return;
            data_state_flag &= 0x00;
            // 存储小屏幕图片
            storage_menu_to_flash(write_menu, (uint8_t *)receive_buff, package_size, 1);
            package_size = 0;
            memset(receive_buff, 0, RE_BUFF_SIZE);
            return;
        }
        // 彩屏图片
        if(color_mode == 0xff) {
            // --------- modify --------------
            uint8_t* temp_arr = (uint8_t *) malloc(sizeof (uint8_t) * (RE_BUFF_SIZE / 2));
            for (uint8_t i = 0; i < (RE_BUFF_SIZE / 2) - 1; i++) {
                temp_arr[i] = string_to_num(receive_buff, i * 2, i * 2 + 1);
                printf("temp_arr -> %x\r\n", temp_arr[i]);
            }
            free(temp_arr);
            temp_arr = NULL;
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
        /*memset(json_str, 0, JSON_SIZE);
        memcpy(json_str, receive_buff, package_size);
        load_parse_key(1);*/
        memset(json_str, 0, JSON_SIZE);
        load_menu_from_flash(write_menu, (uint8_t *)json_str, package_size, 0);
        load_parse_key(1);
        // --------------------------------------------------
        package_size = 0;
        // 清空接收缓存数组
        memset(receive_buff, 0, RE_BUFF_SIZE);
    }
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
