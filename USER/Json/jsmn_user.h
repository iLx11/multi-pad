#ifndef LEDEN_JSMN_USER_H
#define LEDEN_JSMN_USER_H
#define JSMN_HEADER

#include "jsmn.h"
#include "stdio.h"
#include "string.h"
#include "key_user.h"

uint8_t parse_json_data(jsmn_parser *p, uint8_t layer);

void jsmn_init_user();

// 转发执行不同功能
void parse_json_value(uint8_t key_value_index);
#endif //LEDEN_JSMN_USER_H
