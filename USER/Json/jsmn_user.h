#ifndef LEDEN_JSMN_USER_H
#define LEDEN_JSMN_USER_H
#define JSMN_HEADER

#include "jsmn.h"
#include "stdio.h"
#include "string.h"
#include "usb_user.h"
#include "encoder_user.h"

// 键值和小屏幕数组在 FLASH 中占据两个扇区(2 * 4096)，其中键值占据 7168
#define JSON_SIZE 7168

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
// 普通按键 （不包含特殊按键以及延迟的按键）
static void parse_json_normal_key(uint8_t key_value_index);

// 组合按键 （包含特殊按键，不包含延迟的按键）
static void parse_json_comp_key(uint8_t key_value_index);

// 延迟按键 （包含延迟，不包含特殊按键）
static void parse_json_delay_key(uint8_t key_value_index);

// 组合延迟按键 （包含延迟和特殊按键）
static void parse_json_comp_delay_key(uint8_t key_value_index);

static void hid_buff_set_send(uint8_t *start, uint8_t key_value_index, uint8_t special_key);

static void get_execute_delay(uint8_t *start, uint8_t key_value_index);

static void parse_json_mouse_func(uint8_t key_value_index);

static void parse_json_media_func(uint8_t key_value_index);

static void parse_json_menu_func(uint8_t key_value_index);

#endif //LEDEN_JSMN_USER_H
