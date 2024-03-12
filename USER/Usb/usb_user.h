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

void send_hid_code(uint8_t func);
// 发送 hid 重置
void hid_buff_reset();

void receive_data_from_upper(USBD_CUSTOM_HID_HandleTypeDef* hhid, uint8_t len);

static void photo_string_to_hex(const char *hex_string_array);

static void turn_to_next_position();

static uint8_t string_to_num(const char *hex_string_array, uint8_t start, uint8_t end);

static uint8_t char_to_hex(char hex_char);

#endif //LEDEN_USB_USER_H
