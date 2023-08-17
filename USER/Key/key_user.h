#ifndef LEDEN_KEY_USER_H
#define LEDEN_KEY_USER_H

#include "MatrixKeys.h"
#include "usbd_hid.h"
#include "stdio.h"
#include "jsmn_user.h"

void key_init_user();

void key_scan_user();

void send_hid_code(uint8_t func);
#endif //LEDEN_KEY_USER_H
