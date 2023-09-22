#ifndef LEDEN_JSMN_USER_H
#define LEDEN_JSMN_USER_H
#define JSMN_HEADER

#include "jsmn.h"
#include "stdio.h"
#include "string.h"
#include "usb_user.h"
#include "encoder_user.h"

#define JSON_SIZE 4096

extern uint8_t menu_index;

uint8_t parse_json_data(jsmn_parser *p);

void jsmn_init_user();

void load_parse_key(uint8_t menu);

// 转发执行不同功能
void parse_json_value(uint8_t key_value_index);
#endif //LEDEN_JSMN_USER_H
