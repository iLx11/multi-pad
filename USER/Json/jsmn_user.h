#ifndef LEDEN_JSMN_USER_H
#define LEDEN_JSMN_USER_H
#define JSMN_HEADER

#include "jsmn.h"
#include "stdio.h"
#include "string.h"
#include "usb_user.h"
#include "encoder_user.h"

#define JSON_SIZE 12288

extern uint8_t menu_index;

void jsmn_init_user();

uint8_t load_parse_key(uint8_t menu);

static uint8_t parse_json_data(jsmn_parser *p);

// 转发执行不同功能
void parse_json_value(uint8_t key_value_index);

// 数字转字符串
static char *num_to_string(uint8_t num, char *str);

// 字符串转数字
static uint8_t string_to_num_hex(uint8_t key_value_index, uint8_t start, uint8_t end);

/********************************************************************************
* 不同的解析 json 字符串的功能函数
********************************************************************************/
static void parse_json_normal_key(uint8_t key_value_index);

static void parse_json_comp_key(uint8_t key_value_index);

static void parse_json_delay_key(uint8_t key_value_index);

static void parse_json_comp_delay_key(uint8_t key_value_index);

static void hid_buff_set_send(uint8_t *start, uint8_t key_value_index, uint8_t special_key);

static void get_execute_delay(uint8_t *start, uint8_t key_value_index);

static void parse_json_mouse_func(uint8_t key_value_index);

static void parse_json_media_func(uint8_t key_value_index);

static void parse_json_menu_func(uint8_t key_value_index);

#endif //LEDEN_JSMN_USER_H
