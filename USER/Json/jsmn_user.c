#include "jsmn_user.h"

jsmntok_t t[128];
jsmn_parser p;
static const char *json_str = "{\"002\":\"1202015\",\"001\":\"00004\"}";

extern uint8_t send_buff[8];
extern uint8_t special_key_code[8];

// 解析后的键值数组
char key_value_array[20][300] = {0};

// 数字转字符串
static char *num_to_string(uint8_t num, char *str);

// 字符串转数字
static uint8_t string_to_num_hex(uint8_t key_value_index, uint8_t start, uint8_t end);


// 不同的解析 json 字符串的功能函数
static void parse_json_normal_key(uint8_t key_value_index);

static void parse_json_comp_key(uint8_t key_value_index);

// 根据功能解析 json 值的函数指针数组
void (*parse_by_function[4])(uint8_t) = {parse_json_normal_key, parse_json_comp_key};


static uint8_t
special_key_parse(uint8_t key_value_index, uint8_t special_key_start, uint8_t special_key_count, uint8_t func);

void jsmn_init_user() {
    jsmn_init(&p);
    parse_json_data(&p, 0);
}

// 字符键值比对
static int json_cmp(const char *json, jsmntok_t *tok, const char *str) {
    if ((tok->type == JSMN_STRING) &&
        ((int) strlen(str) == tok->end - tok->start) &&
        (strncmp(json + tok->start, str, tok->end - tok->start) == 0))
        return 0;
    return -1;
}

// 解析 json 字符并按键值存入数组
uint8_t parse_json_data(jsmn_parser *p, uint8_t layer) {
    uint8_t r = jsmn_parse(p, json_str, strlen(json_str), t, sizeof(t) / sizeof(t[0]));
    if (r < 0) printf("parse fail");
/*    printf("\ntype - start - end - size\n");
    for (uint8_t i = 0; i < r; i++) {
        printf("\n%3d - %3d - %3d - %3d\n", t[i].type, t[i].start, t[i].end, t[i].size);
    }*/
    // 根据键取字符串
    if (r < 1 && t[0].type != JSMN_OBJECT) return 1;
    for (uint8_t j = 0; j < 20; j++) {
        // jsno 字符的键
        char json_key_str[4] = {0};
        // 数字转字符串
        char key_num_str[3] = {0};
        num_to_string(layer, key_num_str);
        strncat(json_key_str, key_num_str, 1);
        num_to_string(j, key_num_str);
        if (j < 10) strncat(json_key_str, "0", 2);
        strcat(json_key_str, num_to_string(j, key_num_str));

        for (uint8_t s = 0; s < r; s++) {
            if (json_cmp(json_str, &t[s], json_key_str) == 0) {
                // 取出 json 字符中键对应的值
                char json_value_str[250] = {0};
                memcpy(json_value_str, json_str + t[s + 1].start, t[s + 1].end - t[s + 1].start);
                s += 1;
//                printf("json_key_str -> %s\n", json_value_str);
                strcat(key_value_array[j], json_value_str);
                printf("key_value_array[%d] - >%s\n", j, *(key_value_array + j));
            }
        }
    }
    return 0;
}

/********************************************************************************
* 解析 json 字符值的功能位，并根据功能执行
********************************************************************************/
void parse_json_value(uint8_t key_value_index) {
    uint8_t function_index = key_value_array[key_value_index][0] - 0x30;
    printf("function_index -> %d\n", function_index);
    if (function_index > 10 || function_index < 0) return;
    printf("parse_function_running.....\n");
    (*parse_by_function[function_index])(key_value_index);
}

/********************************************************************************
* 解析不同功能的字符串
********************************************************************************/
// 解析 json 字符（普通键值）
static void parse_json_normal_key(uint8_t key_value_index) {
    printf("json_parse_normal_success -> %s\n", key_value_array[key_value_index]);
    send_buff[2] = string_to_num_hex(key_value_index, 2, 4);
}

// 解析 json 字符（组合键值）
static void parse_json_comp_key(uint8_t key_value_index) {
    printf("json_parse_comp_success -> %s\n", key_value_array[key_value_index]);
    uint8_t special_key_count = key_value_array[key_value_index][1] - 0x30;
    do {
        send_buff[0] = special_key_parse(key_value_index, 1, special_key_count, 1);
        for (uint8_t i = 2; i < 8; i++) {
            send_buff[i] = string_to_num_hex(key_value_index, special_key_count + 1 + i, special_key_count + 2 + i);
        }
    } while ((key_value_array[key_value_index][special_key_count + 2] - 6) > 0);

}

// 解析特殊按键
static uint8_t
special_key_parse(uint8_t key_value_index, uint8_t special_key_start, uint8_t special_key_count, uint8_t func) {
    uint8_t result = 0x00;
    if (func == 1) {
        uint8_t edge = special_key_start + special_key_count;
        while (special_key_start < edge) {
            result += special_key_code[key_value_array[key_value_index][special_key_start++] - 0x30];
        }
        printf("special_result -> %d", result);
    } else {
        printf("to do");
    }
    return 0x00 + result;
}


/********************************************************************************
* 数字转字符串
********************************************************************************/
static char *num_to_string(uint8_t num, char *str) {
    uint8_t i = 0;
    if (num < 0) {
        num = -num;
        str[i++] = '-';
    }
    do {
        str[i++] = num % 10 + 48;
    } while (num /= 10);
    str[i] = '\0';
    uint8_t j = 0;
    if (str[0] == '-') {
        j = 1;
        ++i;
    }
    for (; j < i / 2; j++) {
        str[j] = str[j] + str[i - 1 - j];
        str[i - 1 - j] = str[j] - str[i - 1 - j];
        str[j] = str[j] - str[i - 1 - j];
    }
    return str;
}

/********************************************************************************
* 字符串转十六进制数字
********************************************************************************/
static uint8_t string_to_num_hex(uint8_t key_value_index, uint8_t start, uint8_t end) {
    uint8_t result = 0x00;
    result += ((key_value_array[key_value_index][start] - 0x30) * 16);
    result += (key_value_array[key_value_index][end] >= 0x41 && key_value_array[key_value_index][end] <= 0x46) ?
              (0x0A + key_value_array[key_value_index][end] - 0x41) :
              key_value_array[key_value_index][end] - 0x30;
    printf("result -> %d", result);
}
