#include "jsmn_user.h"
#include "flash_user.h"
//#include "fatfs_user.h"

char *json_string = "{\"000\":\"50001\",\"001\":\"50002\",\"002\":\"50004\",\"003\":\"50008\",\"004\":\"50010\",\"05\":\"50020\",\"006\":\"50040\",\"007\":\"50080\"}";

char json_str[JSON_SIZE];

// hid 发送缓冲数组指针结构体
extern buff_struct buff_point_array[3];

jsmntok_t t[128];
jsmn_parser p;

// 解析后的键值数组
char key_value_array[30][200] = {0};

// 数字转字符串
static char *num_to_string(uint8_t num, char *str);

// 字符串转数字
static uint8_t string_to_num_hex(uint8_t key_value_index, uint8_t start, uint8_t end);

// 不同的解析 json 字符串的功能函数
static void parse_json_normal_key(uint8_t key_value_index);

static void parse_json_comp_key(uint8_t key_value_index);

static void parse_json_delay_key(uint8_t key_value_index);

static void parse_json_comp_delay_key(uint8_t key_value_index);

static void hid_buff_set_send(uint8_t *start, uint8_t key_value_index, uint8_t special_key);

static void get_execute_delay(uint8_t *start, uint8_t key_value_index);

static void parse_json_mouse_func(uint8_t key_value_index);

static void parse_json_media_func(uint8_t key_value_index);

static void parse_json_menu_func(uint8_t key_value_index);

// 根据功能解析 json 值的函数指针数组
void (*parse_by_function[8])(uint8_t) = {parse_json_normal_key, parse_json_comp_key, parse_json_delay_key,
                                         parse_json_comp_delay_key, parse_json_mouse_func,
                                         parse_json_media_func, parse_json_menu_func};

// json 解析初始化
void jsmn_init_user() {
    jsmn_init(&p);
    parse_json_data(&p, 0);
}

/********************************************************************************
* 加载键值配置并解析在键值数组
********************************************************************************/
void load_parse_key(uint8_t menu) {
    // 从 flash 加载键值
    load_setting_from_flash(menu, (uint8_t *) json_string, JSON_SIZE);
    // 解析键值
    parse_json_data(&p, menu);
}

/********************************************************************************
* 字符键值比对
********************************************************************************/
static int json_cmp(const char *json, jsmntok_t *tok, const char *str) {
    if ((tok->type == JSMN_STRING) &&
        ((int) strlen(str) == tok->end - tok->start) &&
        (strncmp(json + tok->start, str, tok->end - tok->start) == 0))
        return 0;
    return -1;
}

/********************************************************************************
* 解析 json 字符并按键值存入数组
********************************************************************************/
uint8_t parse_json_data(jsmn_parser *p, uint8_t layer) {
    uint8_t r = jsmn_parse(p, json_string, strlen(json_string), t, sizeof(t) / sizeof(t[0]));
    if (r < 0) printf("parse fail");
/*    printf("\ntype - start - end - size\n");
    for (uint8_t i = 0; i < r; i++) {
        printf("\n%3d - %3d - %3d - %3d\n", t[i].type, t[i].start, t[i].end, t[i].size);
    }*/
    // 根据键取字符串
    if (r < 1 && t[0].type != JSMN_OBJECT) return 1;
    for (uint8_t j = 0; j < 30; j++) {
        // sprintf 函数执行速度还未测试
        // json 字符的键
        char json_key_str[4] = {0};
        // 数字转字符串
        char key_num_str[3] = {0};
        num_to_string(layer, key_num_str);
        strncat(json_key_str, key_num_str, 1);
        num_to_string(j, key_num_str);
        if (j < 10) strncat(json_key_str, "0", 2);
        strcat(json_key_str, num_to_string(j, key_num_str));

        for (uint8_t s = 0; s < r; s++) {
            if (json_cmp(json_string, &t[s], json_key_str) == 0) {
                // 取出 json 字符中键对应的值
                char json_value_str[200] = {0};
                memcpy(json_value_str, json_string + t[s + 1].start, t[s + 1].end - t[s + 1].start);
                s += 1;
//                printf("json_key_str -> %s\n", json_value_str);
                strcat(key_value_array[j], json_value_str);
                printf("key_value_array[%d] - >%s\n\r", j, *(key_value_array + j));
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
* ------ 解析不同功能的字符串 ------ start
********************************************************************************/

/********************************************************************************
* 解析 json 字符（普通键值）
********************************************************************************/
static void parse_json_normal_key(uint8_t key_value_index) {
    uint8_t start = 1;
    hid_buff_set_send(&start, key_value_index, 0x00);
    /*uint8_t normal_key_count = string_to_num_hex(key_value_index, start++, start++);
    uint8_t cycle_num = normal_key_count / 6;
    do {
        buff_point_array[0].send_buff_point[0] = 0x01;
        for (uint8_t i = 0; i < 6; i++) {
            buff_point_array[0].send_buff_point[i + 3] = string_to_num_hex(key_value_index, start++, start++);
        }
        send_hid_code(0);
        hid_buff_reset();
    } while (cycle_num--);*/
}

/********************************************************************************
* 解析 json （组合组合）
********************************************************************************/
static void parse_json_comp_key(uint8_t key_value_index) {
    uint8_t start = 1;
    uint8_t compp_key_count = string_to_num_hex(key_value_index, start++, start++);
    while (compp_key_count--) {
        uint8_t special_key = string_to_num_hex(key_value_index, start++, start++);
        hid_buff_set_send(&start, key_value_index, special_key);
    }
}

/********************************************************************************
* 解析 json (普通延迟按键）
********************************************************************************/
static void parse_json_delay_key(uint8_t key_value_index) {
//    printf("json_parse_normal_success -> %s\n", key_value_array[key_value_index]);
    uint8_t delay_key_count = key_value_array[key_value_index][1] - 0x30;
    uint8_t start = 2;
    hid_buff_set_send(&start, key_value_index, 0x00);
    while (delay_key_count--) {
        get_execute_delay(&start, key_value_index);
        hid_buff_set_send(&start, key_value_index, 0x00);
    }
}

/********************************************************************************
* 解析 json (组合延迟按键）
********************************************************************************/
static void parse_json_comp_delay_key(uint8_t key_value_index) {
    uint8_t delay_key_count = key_value_array[key_value_index][1] - 0x30;
    uint8_t start = 2;
    uint8_t special_key = string_to_num_hex(key_value_index, start++, start++);
    hid_buff_set_send(&start, key_value_index, special_key);
    while (delay_key_count--) {
        get_execute_delay(&start, key_value_index);
        buff_point_array[0].send_buff_point[1] = string_to_num_hex(key_value_index, start++, start++);
        hid_buff_set_send(&start, key_value_index);
    }
}

/********************************************************************************
* 设置与发送缓冲数组
********************************************************************************/
static void hid_buff_set_send(uint8_t *start, uint8_t key_value_index, uint8_t special_key) {
    buff_point_array[0].send_buff_point[0] = 0x01;
    uint8_t normal_key_count = string_to_num_hex(key_value_index, (*start)++, (*start)++);
    uint8_t cycle_num = normal_key_count / 6;
    do {
        buff_point_array[0].send_buff_point[1] = special_key;
        for (uint8_t i = 0; i < 6; i++){
            if(normal_key_count == 0) break;
            buff_point_array[0].send_buff_point[i + 3] = string_to_num_hex(key_value_index, (*start)++, (*start)++);
            normal_key_count --;
        }
        send_hid_code(0);
        hid_buff_reset();
    } while (cycle_num--);
}

/********************************************************************************
* 获取与执行延迟
********************************************************************************/
static void get_execute_delay(uint8_t *start, uint8_t key_value_index) {
    uint16_t delay_time = (key_value_array[key_value_index][(*start)++] - 0x30) * 1000;
    delay_time += string_to_num_hex(key_value_index, (*start)++, (*start)++);
    HAL_Delay(delay_time);
}

/********************************************************************************
* 解析json (鼠标功能）
********************************************************************************/
static void parse_json_mouse_func(uint8_t key_value_index) {
    buff_point_array[1].send_buff_point[0] = 0x02;
    buff_point_array[1].send_buff_point[1] = string_to_num_hex(key_value_index, 1, 2);
    char sign;
    for (uint8_t i = 2, start = 3; i < 5; i++) {
        sign = key_value_array[key_value_index][start++] - 0x30 == 1 ? 1 : -1;
        buff_point_array[1].send_buff_point[i] = string_to_num_hex(key_value_index, start++, start++) * sign;
    }
    send_hid_code(1);
    hid_buff_reset();
}

/********************************************************************************
* 媒体功能
********************************************************************************/
static void parse_json_media_func(uint8_t key_value_index) {
    buff_point_array[2].send_buff_point[0] = 0x03;
    buff_point_array[2].send_buff_point[1] = string_to_num_hex(key_value_index, 1, 2);
    buff_point_array[2].send_buff_point[2] = string_to_num_hex(key_value_index, 3, 4);
    send_hid_code(2);
    hid_buff_reset();
}

/********************************************************************************
* 菜单切换功能
********************************************************************************/
static void parse_json_menu_func(uint8_t key_value_index) {
    uint8_t menu_func = key_value_array[key_value_index][1] - 0x30;
    if (menu_func == 0)
        menu_index <= 0 ? menu_index = 9 : menu_index--;
    else if (menu_func == 1)
        menu_index >= 9 ? menu_index = 0 : menu_index++;
    else
        menu_index = key_value_array[key_value_index][2] - 0x30;
    debounce_func(key_value_index);
}

/********************************************************************************
* ------ 解析不同功能的字符串 ------ end
********************************************************************************/


/********************************************************************************
* ------ 工具函数 ------ start
********************************************************************************/

/********************************************************************************
* 字符串转十六进制数字
********************************************************************************/
static uint8_t string_to_num_hex(uint8_t key_value_index, uint8_t start, uint8_t end) {
    uint8_t result = 0x00;
    result += ((key_value_array[key_value_index][start] - 0x30) * 16);
    result += (key_value_array[key_value_index][end] >= 0x41 && key_value_array[key_value_index][end] <= 0x46) ?
              (0x0A + key_value_array[key_value_index][end] - 0x41) :
              key_value_array[key_value_index][end] - 0x30;
    return result;
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
* ------ 工具函数 ------ end
********************************************************************************/
