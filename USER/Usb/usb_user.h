#ifndef LEDEN_USB_USER_H
#define LEDEN_USB_USER_H

#include "usbd_customhid.h"
#include "usbd_cdc.h"
#include <stdio.h>

// 缓冲数组指针结构体
typedef struct {
    uint8_t *send_buff_point;
    uint8_t *zero_buff_point;
    uint8_t buff_size;
} buff_struct;

void usb_init_user(void);

void usb_scan_user(void);

void send_hid_code(uint8_t func);
// 发送 hid 重置
void hid_buff_reset(void);

static void receive_reset(void);

static uint8_t string_to_num(const char *hex_string_array, uint8_t start, uint8_t end);

static uint8_t char_to_hex(char hex_char);

#endif //LEDEN_USB_USER_H
