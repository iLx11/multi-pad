#ifndef LEDEN_USB_USER_H
#define LEDEN_USB_USER_H

#include "usbd_customhid.h"
#include "usbd_cdc.h"
#include <stdio.h>

#define MAX_PACK_SIZE (62)
#define RE_BUFF_SIZE (4096)

#define SIGN_SET 1
#define SIGN_RESET 0

// 缓冲数组结构体类型
typedef struct usb_data {
    // 缓冲数组指针结构体
    struct {
        uint8_t *key_p;
        uint8_t *zero_p;
        uint8_t buff_size;
    } data_buff[3];
}* p_usb_data_t;

// 信号数据结构体类型
typedef struct sign_data {
    uint8_t sign_byte;
    enum {
        // 接收
        data_rec = 0,
        // 数据状态
        data_state,
        // 菜单配置
        menu_config,
        // 键值数据状态 
        key_state,
        // 彩屏数据模式
        color_mode
    } sign_bit;
}* p_sign_data_t;

// 上位机交互数据结构体类型
typedef struct upper_data {
    // 数据包数量
    uint8_t package_num;
    // 接收缓存数组
    uint8_t* rec_buff;
    // 数据包的大小
    uint16_t package_size;
    // 彩屏数据包计算
    uint32_t color_package_count;
}* p_upper_data_t;

uint8_t cdc_event_cb(uint8_t *Buf, uint32_t *Len);

void usb_init_user(void);

void first_load_menu(void);

void load_menu(uint8_t menu_index);

void set_data_buff(uint8_t func, uint8_t pos, uint8_t data);

void usb_scan_user(void);

static uint8_t get_sign(uint8_t shift);

static void set_sign(uint8_t shift, uint8_t value);

void load_menu_config(void);

void turn_next_menu(void);

void send_hid_code(uint8_t func);
// 发送 hid 重置
void hid_buff_reset(void);

static void receive_reset(void);

static uint8_t string_to_num(const char *hex_string_array, uint8_t start, uint8_t end);

static uint8_t char_to_hex(char hex_char);

#endif //LEDEN_USB_USER_H
