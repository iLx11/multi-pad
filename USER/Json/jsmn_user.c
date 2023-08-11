#include "jsmn_user.h"

jsmntok_t t[128];
jsmn_parser p;
static const char *json_str = "{\"002\":\"21023034056\",\"202\":\"2102303405asdfaskldfjaskldfjasklfjaskl;djf6\"}";

// 解析后的键值数组
static char* key_value_array[200] = {0};

static char *num_to_string(uint8_t num, char *str);

void jsmn_init_user() {
    jsmn_init(&p);
    parse_json_data(&p);
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
uint8_t parse_json_data(jsmn_parser *p) {
    uint8_t r = jsmn_parse(p, json_str, strlen(json_str), t, sizeof(t) / sizeof(t[0]));
    if (r < 0) printf("parse fail");
/*    printf("\ntype - start - end - size\n");
    for (uint8_t i = 0; i < r; i++) {
        printf("\n%3d - %3d - %3d - %3d\n", t[i].type, t[i].start, t[i].end, t[i].size);
    }*/
    // 根据键取字符串
    if (r < 1 && t[0].type != JSMN_OBJECT) return 1;
    for (uint8_t i = 0; i < 10; i++) {
        for (uint8_t j = 0; j < 20; j++) {
            // jsno 字符的键
            char json_key_str[4] = {0};
            // 数字转字符串
            char key_num_str[3] = {0};
            num_to_string(i, key_num_str);
            strncat(json_key_str, key_num_str, 1);
            num_to_string(j, key_num_str);
            if (j < 10) strncat(json_key_str, "0", 2);
            strcat(json_key_str, num_to_string(j, key_num_str));

            for (uint8_t s = 0; s < r; s++) {
                if (json_cmp(json_str, &t[s], json_key_str) == 0) {
                    // 取出 json 字符中键对应的值
                    char json_value_str[300] = {0};
                    memcpy(json_value_str, json_str + t[s + 1].start, t[s + 1].end - t[s + 1].start);
                    s += 1;
                    printf("json_key_str -> %s", json_value_str);
                    memcpy((*key_value_array + i) + j, json_value_str, strlen(json_value_str));
                    printf("%s", (*key_value_array + i));
                }
            }
        }
    }
    return 0;
}

/********************************************************************************
* 数字转字符
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
* 解析不同功能的字符串
********************************************************************************/
// 解析 json 字符（普通键值）
static void parse_json_normal_key(const char *json_str_key, const char *json_str_value) {
    uint8_t aa = json_str_key[0] - 0x30;
    printf("aa = %d", aa);
}

// 解析 json 字符（组合键值）
static void parse_json_comp_key() {

}