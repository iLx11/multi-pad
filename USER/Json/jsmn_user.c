#include "jsmn_user.h"
#include "flash_user.h"

// hid ���ͻ�������ָ��ṹ��
extern buff_struct buff_point_array[3];

extern uint8_t menu_change_lock;

//char *json_str = "{\"00\":\"101000B171A08171A08222021221F\",\"04\":\"0051715081A09\"}";
char json_str[JSON_SIZE];

//jsmntok_t t[512];
jsmn_parser p;


// ������ļ�ֵ���� (2 * 8) + (6 * 3)
char key_value_array[EVENT_NUM][EVENT_SIZE] = {0};

// ���ݹ��ܽ��� json ֵ�ĺ���ָ������
void (*parse_by_function[8])(uint8_t) = {parse_json_normal_key, parse_json_comp_key, parse_json_delay_key,
                                         parse_json_comp_delay_key, parse_json_mouse_func,
                                         parse_json_media_func, parse_json_menu_func};

// json ������ʼ��
void jsmn_init_user() {
    jsmn_init(&p);
    menu_change_lock = 1;
    load_parse_key(1);
}

/********************************************************************************
* ���ؼ�ֵ���ò������ڼ�ֵ����
********************************************************************************/
uint8_t load_parse_key(uint8_t menu) {
    // ����ַ���
    memset(key_value_array, 0, sizeof (key_value_array));
    // �� flash ���ؼ�ֵ
//    load_menu_from_flash(menu, (uint8_t *) json_str, 12288, 1);
    // ��� jsmn �����ṹ��
    memset(&p, 0, sizeof(jsmn_parser));
    // ������ֵ
    return parse_json_data(&p);
}

/********************************************************************************
* �ַ���ֵ�ȶ�
********************************************************************************/
static int json_cmp(const char *json, jsmntok_t *tok, const char *str) {
    if ((tok->type == JSMN_STRING) &&
        ((int) strlen(str) == tok->end - tok->start) &&
        (strncmp(json + tok->start, str, tok->end - tok->start) == 0))
        return 0;
    return -1;
}

/********************************************************************************
* ���� json �ַ�������ֵ��������
********************************************************************************/
static uint8_t parse_json_data(jsmn_parser *p) {
    jsmntok_t t[512] = {0};
    uint8_t r = jsmn_parse(p, json_str, strlen(json_str), t, sizeof(t) / sizeof(t[0]));
    if (r < 0)  {
        printf("parse fail");
        return 0;
    }
    // ���ݼ�ȡ�ַ���+
    if (r < 1 && t[0].type != JSMN_OBJECT) return 1;
    for (uint8_t j = 0; j < EVENT_NUM; j++) {
        // sprintf ����ִ���ٶȻ�δ����
        // json �ַ��ļ�
        char json_key_str[4] = {0};
        // ����ת�ַ���
        char key_num_str[3] = {0};
//        strcat(json_key_str, "l");
        if (j < 10) strcat(json_key_str, "0");
        strcat(json_key_str, num_to_string(j, key_num_str));
        for (uint8_t s = 0; s < r; s++) {
            if (json_cmp(json_str, &t[s], json_key_str) == 0) {
                // ȡ�� json �ַ��м���Ӧ��ֵ
                char json_value_str[EVENT_SIZE] = {0};
                memcpy(json_value_str, json_str + t[s + 1].start, t[s + 1].end - t[s + 1].start);
                s += 1;
                strcat(key_value_array[j], json_value_str);
                printf("key_value_array[%d] - >%s\n\r", j, *(key_value_array + j));
            }
        }
        menu_change_lock = 0;
    }
    return 1;
}

/********************************************************************************
* ���� json �ַ�ֵ�Ĺ���λ�������ݹ���ִ��
********************************************************************************/
void parse_json_value(uint8_t key_value_index) {
    uint8_t function_index = key_value_array[key_value_index][0] - 0x30;
    printf("function_index -> %d\n", function_index);
    if (function_index > 10 || function_index < 0) return;
    printf("parse_function_running.....\n");
    holding_flag = 0;
    printf("holding_flag -> %d\r\n", holding_flag);
    (*parse_by_function[function_index])(key_value_index);

}

/********************************************************************************
* ------ ������ͬ���ܵ��ַ��� ------ start
********************************************************************************/

/********************************************************************************
* ���� json �ַ�����ͨ��ֵ��
********************************************************************************/
static void parse_json_normal_key(uint8_t key_value_index) {
    uint8_t start = 1;
    hid_buff_set_send(&start, key_value_index, 0x00);
}

/********************************************************************************
* ���� json �������ϣ�
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
* ���� json (��ͨ�ӳٰ�����
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
* ���� json (����ӳٰ�����
********************************************************************************/
static void parse_json_comp_delay_key(uint8_t key_value_index) {
    uint8_t delay_key_count = key_value_array[key_value_index][1] - 0x30;
    uint8_t start = 2;
    uint8_t special_key_count = string_to_num_hex(key_value_index, start++, start++);
    uint8_t special_key;
    while(special_key_count --) {
        special_key = string_to_num_hex(key_value_index, start++, start++);
        hid_buff_set_send(&start, key_value_index, special_key);
    }
    while (delay_key_count--) {
        get_execute_delay(&start, key_value_index);
        special_key_count = string_to_num_hex(key_value_index, start++, start++);
        while (special_key_count --) {
            special_key = string_to_num_hex(key_value_index, start++, start++);
            hid_buff_set_send(&start, key_value_index, special_key);
        }
    }
}

/********************************************************************************
* �����뷢�ͻ�������
********************************************************************************/
static void hid_buff_set_send(uint8_t *start, uint8_t key_value_index, uint8_t special_key) {
    uint8_t normal_key_count = string_to_num_hex(key_value_index, (*start)++, (*start)++);
    uint8_t cycle_num = normal_key_count / 6;
    do {
        buff_point_array[0].send_buff_point[0] = 0x01;
        buff_point_array[0].send_buff_point[1] = special_key;
        for (uint8_t i = 0; i < 6; i++){
            if(normal_key_count == 0) break;
            buff_point_array[0].send_buff_point[i + 3] = string_to_num_hex(key_value_index, (*start)++, (*start)++);
            normal_key_count --;
        }
//        for(uint8_t i = 0; i < 9; i ++) {
//            printf("\nsend_buff[%d] -> %02x\n\r", i, buff_point_array[0].send_buff_point[i]);
//        }
        send_hid_code(0);
        hid_buff_reset();
    } while (cycle_num--);
}

/********************************************************************************
* ��ȡ��ִ���ӳ�
********************************************************************************/
static void get_execute_delay(uint8_t *start, uint8_t key_value_index) {
    uint16_t delay_time = string_to_num_hex(key_value_index, (*start)++, (*start)++) * 255;
    delay_time += string_to_num_hex(key_value_index, (*start)++, (*start)++);
    HAL_Delay(delay_time);
}

/********************************************************************************
* ����json (��깦�ܣ�
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
* ý�幦��
********************************************************************************/
static void parse_json_media_func(uint8_t key_value_index) {
    buff_point_array[2].send_buff_point[0] = 0x03;
    buff_point_array[2].send_buff_point[1] = string_to_num_hex(key_value_index, 1, 2);
    buff_point_array[2].send_buff_point[2] = string_to_num_hex(key_value_index, 3, 4);
    send_hid_code(2);
    hid_buff_reset();
}

/********************************************************************************
* �˵��л�����
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
* ------ ������ͬ���ܵ��ַ��� ------ end
********************************************************************************/


/********************************************************************************
* ------ ���ߺ��� ------ start
********************************************************************************/

/********************************************************************************
* �ַ���תʮ����������
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
* ����ת�ַ���
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
* ------ ���ߺ��� ------ end
********************************************************************************/
