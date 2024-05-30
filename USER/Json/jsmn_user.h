#ifndef LEDEN_JSMN_USER_H
#define LEDEN_JSMN_USER_H
#define JSMN_HEADER

#include "jsmn.h"
#include "stdio.h"
#include "string.h"
#include "usb_user.h"
#include "encoder_user.h"

// ��ֵ��С��Ļ������ FLASH ��ռ����������(2 * 4096)�����м�ֵռ�� 7168
#define JSON_SIZE 7168

#define EVENT_NUM 34

#define EVENT_SIZE 456

// ������ֵ�ĺ������Ͷ���
typedef void (*parse_func_t)(uint8_t);

// json ���ܽṹ�嶨��
typedef struct jsmn_func {
    // json ����
    char* json_str;
    // ������ļ�ֵ����
    char** key_value_arr;
    // ���ݹ��ܽ��� json ֵ�ĺ���ָ������
    parse_func_t parse_func[8];
} *p_jsmn_t;

void jsmn_init_user();

uint8_t load_parse_key(uint8_t menu);

static uint8_t parse_json_data(jsmn_parser *p);

// ת��ִ�в�ͬ����
void parse_json_value(uint8_t key_value_index);

// ����ת�ַ���
static char *num_to_string(uint8_t num, char *str);

// �ַ���ת����
static uint8_t string_to_num_hex(uint8_t key_value_index, uint8_t start, uint8_t end);

/********************************************************************************
* ��ͬ�Ľ��� json �ַ����Ĺ��ܺ���
********************************************************************************/
// ��ͨ���� �����������ⰴ���Լ��ӳٵİ�����
static void parse_json_normal_key(uint8_t key_value_index);

// ��ϰ��� ���������ⰴ�����������ӳٵİ�����
static void parse_json_comp_key(uint8_t key_value_index);

// �ӳٰ��� �������ӳ٣����������ⰴ����
static void parse_json_delay_key(uint8_t key_value_index);

// ����ӳٰ��� �������ӳٺ����ⰴ����
static void parse_json_comp_delay_key(uint8_t key_value_index);

static void hid_buff_set_send(uint8_t *start, uint8_t key_value_index, uint8_t special_key);

static void get_execute_delay(uint8_t *start, uint8_t key_value_index);

static void parse_json_mouse_func(uint8_t key_value_index);

static void parse_json_media_func(uint8_t key_value_index);

static void parse_json_menu_func(uint8_t key_value_index);

#endif //LEDEN_JSMN_USER_H
