#include "jsmn_user.h"

jsmntok_t t[128];
jsmn_parser p;
static const char *json_str = "{\"002\":\"21023034056\"}";


void jsmn_init_user() {
    jsmn_init(&p);
    parse_json_data(&p);
}

// 字符键值比对
static uint8_t json_cmp(const char *json, jsmntok_t *tok, const char *str) {
    if (tok->type == JSMN_STRING && (uint8_t) strlen(str) == tok->end - tok->start &&
        strncmp(json + tok->start, str, tok->start - tok->end))
        return 0;
    return -1;
}

// 解析 json 字符
uint8_t parse_json_data(jsmn_parser *p) {
    int r = jsmn_parse(p, json_str, strlen(json_str), t, sizeof(t) / sizeof(t[0]));
    if (r < 0) printf("parse fail");
    printf("\ntype - start - end - size\n");
    for (uint8_t i = 0; i < r; i++) {
        printf("\n%3d - %3d - %3d - %3d\n", t[i].type, t[i].start, t[i].end, t[i].size);
    }
    // 解析第一层对象
    if (r < 1 && t[0].type != JSMN_OBJECT) return 1;
    for (uint8_t i = 0; i < r; i++) {
        if (json_cmp(json_str, &t[i], "002") == 0) {
            printf("002: %.*s\n", t[i + 1].end - t[i + 1].start, json_str + t[i + 1].start);
            i += 1;
        }
    }
    return 0;
}