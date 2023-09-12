#ifndef LEDEN_USB_USER_H
#define LEDEN_USB_USER_H

#include "usbd_customhid.h"
#include <stdio.h>

// ��������ָ��ṹ��
typedef struct {
    uint8_t *send_buff_point;
    uint8_t *zero_buff_point;
    uint8_t buff_size;
} buff_struct;

void send_hid_code(uint8_t func);
// ���� hid ����
void hid_buff_reset();
#endif //LEDEN_USB_USER_H