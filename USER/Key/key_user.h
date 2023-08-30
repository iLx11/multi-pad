#ifndef LEDEN_KEY_USER_H
#define LEDEN_KEY_USER_H

#include "MatrixKeys.h"
#include "usbd_customhid.h"
#include "stdio.h"
#include "jsmn_user.h"

// ��������ָ��ṹ��
typedef struct {
    uint8_t *send_buff_point;
    uint8_t *zero_buff_point;
    uint8_t buff_size;
} buff_struct;

void key_init_user();

void key_scan_user();

void send_hid_code(uint8_t func);

// ���� hid ����
void hid_buff_reset();
#endif //LEDEN_KEY_USER_H
