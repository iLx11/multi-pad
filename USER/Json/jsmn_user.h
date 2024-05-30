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

#define EVENT_NUM 34

#define EVENT_SIZE 456

// 解析键值的函数类型定义
typedef void (*parse_func_t)(uint8_t);

// json 功能结构体定义
typedef struct jsmn_func {
    // json 数据
    char* json_str;
    // 解析后的键值数组
    char** key_value_arr;
    // 根据功能解析 json 值的函数指针数组
    parse_func_t parse_func[8];
} *p_jsmn_t;

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
