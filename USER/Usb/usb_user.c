#include "usb_user.h"
#include "oled_user.h"
#include "flash_user.h"
#include "usbd_customhid.h"
#include "jsmn_user.h"
#include "usbd_cdc_if.h"
#include "lcd_user.h"
#include "oled_42.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

// hid ����
// ����
uint8_t send_key_buff[9] = {0};
uint8_t send_key_zero_buff[9] = {0x01, 0, 0, 0, 0, 0, 0, 0, 0};
// ���
uint8_t send_mouse_buff[5] = {0};
uint8_t send_mouse_zero_buff[5] = {0x02, 0, 0, 0, 0};
// ý��
uint8_t send_media_buff[5] = {0};
uint8_t send_media_zero_buff[5] = {0x03, 0x00, 0x00, 0x00, 0x00};

// ��ǰ�� buff
uint8_t cur_buff = 0;

buff_struct buff_point_array[3] = {
        {.send_buff_point = send_key_buff, .zero_buff_point = send_key_zero_buff, .buff_size = 9},
        {.send_buff_point = send_mouse_buff, .zero_buff_point = send_mouse_zero_buff, .buff_size = 5},
        {.send_buff_point = send_media_buff, .zero_buff_point = send_media_zero_buff, .buff_size = 5}
};

// ��������
//extern char json_str[JSON_SIZE];

#define RE_BUFF_SIZE 4096

// �˵�����
uint8_t write_menu = 1;
// ���� FLAG
uint8_t data_receive_flag;
// ����״̬
uint8_t data_state_flag;
// �˵�����
uint8_t menu_config;
// ���ջ�������
uint8_t receive_buff[RE_BUFF_SIZE];
// ���ݰ��Ĵ�С
uint16_t package_size;
// ����״̬
uint8_t data_state;
// ��������ģʽ
uint8_t color_mode;
// �������ݰ�����
uint32_t color_package_count = 0;
// �ȴ��ź�
uint8_t sign[1] = {0x68};
// �˵���������
uint8_t menu_config_arr[31] = {0};
// �˵���
extern uint8_t menu_change_lock;

/********************************************************************************
* ��������
********************************************************************************/
void first_load_menu(void) {
    // ���ز˵�����
    load_menu_config();
    if (menu_config_arr[0] >= 1 && menu_config_arr[0] <= 10) {
        load_menu(menu_config_arr[0]);
    } else {
        uint8_t *temp = (uint8_t *) calloc(sizeof(uint8_t), 31);
        temp[0] = 1;
        storage_config(temp, 31);
        load_menu(1);
    }
}


/********************************************************************************
* ���ز˵�
********************************************************************************/
void load_menu(uint8_t menu_index) {
    if (menu_config_arr[menu_index] != 1) return;
    menu_change_lock = 1;
    load_parse_key(menu_index);
    show_menu_oled(menu_index);
    show_menu_color(menu_index);
    menu_change_lock = 0;
}

/********************************************************************************
* USB ɨ��
********************************************************************************/
void usb_scan_user(void) {
    // ��������ջ�������֮��д�� FLASH
    if (data_receive_flag == 0xff) {
        data_receive_flag &= 0x00;
        // д��˵�
        if (menu_config == 0xff) {
            menu_config &= 0x00;
            storage_config((uint8_t *) receive_buff, 31);
            load_menu_config();
            receive_reset();
            CDC_Transmit_FS(sign, 1);
            return;
        }
        // д���ֵ��ͼƬ
        // ����ͼƬ
        if (data_state_flag == 0xff) {
            if (package_size > 730) return;
            data_state_flag &= 0x00;
            // תΪͼƬ���ݺ�洢�� FLASH
            storage_menu_to_flash(write_menu, (uint8_t *) receive_buff, package_size, 1);
            // ------------- test --------------
//            show_menu_oled(write_menu);
            data_state &= 0x00;
            receive_reset();
            CDC_Transmit_FS(sign, 1);
            return;
        }
        // ����ͼƬ
        if (color_mode == 0xff) {
            // �洢ͼƬ
            storage_color_screen(write_menu, color_package_count << 12, receive_buff, package_size);
            // ͼƬ��������ж�
            if (color_package_count < 26) {
                color_package_count++;
            } else {
                // ͼƬ�������
                color_package_count = 0;
                // --------------- test -----------------
                color_mode &= 0x00;
                load_menu(write_menu);
//                write_menu ++;
//                turn_next_menu();
            }
            receive_reset();
            CDC_Transmit_FS(sign, 1);
            return;
        }
        // д���ֵ
        if (data_state == 0xff) {
            // ��ֵ�洢�ĵڶ���״̬
            data_state &= 0x00;
            storage_page_two(write_menu, (uint8_t *) &receive_buff, package_size);
        } else {
            // ��ֵ�洢�ĵ�һ��״̬
            data_state |= 0xff;
            storage_menu_to_flash(write_menu, (uint8_t *) &receive_buff, package_size, 0);
        }

        // ------------------ test --------------------------
        /*memset(json_str, 0, JSON_SIZE);
        memcpy(json_str, receive_buff, package_size);
        load_parse_key(1);*/
        /*memset(json_str, 0, JSON_SIZE);
        load_menu_from_flash(write_menu, (uint8_t *)json_str, package_size, 0);
        load_parse_key(1);*/
        // --------------------------------------------------
        receive_reset();
        CDC_Transmit_FS(sign, 1);

    }
}

/********************************************************************************
* ���ز˵�����
********************************************************************************/
void load_menu_config(void) {
    uint8_t *temp = (uint8_t *) malloc(sizeof(uint8_t) * 31);
    load_config(temp, 31);
    memcpy((uint8_t *) menu_config_arr, (uint8_t *) temp, 31);
    for(uint8_t i = 0; i < 31; i ++) {
        printf("temp -> %d", menu_config_arr[i]);
    }
    free(temp);
    temp = NULL;
}


/********************************************************************************
* ��ת�������ݵ���һ���˵�
********************************************************************************/
void turn_next_menu(void) {
    while (menu_config_arr[write_menu] == 0x00) {
        if (write_menu > 10) {
            write_menu = 1;
            return;
        }
        write_menu += 1;
    }
}

/********************************************************************************
* ��ȡ��ֵ��ҳ��ֵ�ĳ���
********************************************************************************/
uint32_t get_key_size(uint8_t index) {
    uint8_t p = (index * 2) + 10;
    return menu_config_arr[p - 1] * 255 + menu_config_arr[p];
}

/********************************************************************************
* ���ñ�־
********************************************************************************/
static void receive_reset(void) {
    package_size = 0;
    // ��ս��ջ�������
    memset(receive_buff, 0, RE_BUFF_SIZE);
}

/********************************************************************************
* ���� hid ��
********************************************************************************/
void send_hid_code(uint8_t func) {
    cur_buff = func;
    while (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, buff_point_array[func].send_buff_point,
                                      buff_point_array[func].buff_size) != USBD_OK);
}

/********************************************************************************
* ���� hid ����
********************************************************************************/
void hid_buff_reset(void) {
    for (uint8_t i = 0; i < buff_point_array[cur_buff].buff_size; i++) {
        buff_point_array[cur_buff].send_buff_point[i] &= 0x00;
    }
    while (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, buff_point_array[cur_buff].zero_buff_point,
                                      buff_point_array[cur_buff].buff_size) != USBD_OK);
    printf("reset ok\n");
}

/********************************************************************************
* �ַ���תʮ������
********************************************************************************/
static uint8_t string_to_num(const char *hex_string_array, uint8_t start, uint8_t end) {
    return char_to_hex(hex_string_array[start]) * 16 + char_to_hex(hex_string_array[end]);
}

/********************************************************************************
* �ַ�תʮ������
********************************************************************************/
static uint8_t char_to_hex(char hex_char) {
    uint8_t result = 0x00;
    if (hex_char >= '0' && hex_char <= '9')
        result += (hex_char - 0x30);
    else if (hex_char >= 'a' && hex_char <= 'f')
        result += (0x0A + hex_char - 0x61);
    else if (hex_char >= 'A' && hex_char <= 'F') {
        result += (0x0A + hex_char - 0x41);
    }
    return result;
}
