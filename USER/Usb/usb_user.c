#include "usb_user.h"
#include "oled_user.h"
#include "flash_user.h"
#include "usbd_customhid.h"
#include "jsmn_user.h"
#include "usbd_cdc_if.h"
#include "lcd_user.h"
#include "oled_42.h"

// typedef unsigned char uint8_t 

extern USBD_HandleTypeDef hUsbDeviceFS;
// usb ��ֵ����
static p_usb_data_t usb_data_p = NULL;
// �ź�����
static p_sign_data_t sign_data_p = NULL;
// ��λ����������
static p_upper_data_t upper_data_p = NULL;

// ����
static uint8_t key_buff[9] = {0};
static uint8_t key_zero[9] = {0x01};
// ���
static uint8_t mouse_buff[5] = {0};
static uint8_t mouse_zero[5] = {0x02};
// ý��
static uint8_t media_buff[5] = {0};
static uint8_t media_zero[5] = {0x03};

// ��ǰ�� buff
static uint8_t cur_buff = 0;
// �˵���������
static uint8_t menu_config_arr[31] = {0};
// �˵�����
static uint8_t write_menu = 1;

// ���ջ�������
static uint8_t rec_buff[RE_BUFF_SIZE];

/********************************************************************************
* CDC ����ص�
********************************************************************************/
uint8_t cdc_event_cb(uint8_t *Buf, uint32_t *Len) {
    // �ж�Խ��
    if (upper_data_p->package_size >= RE_BUFF_SIZE || *Len > MAX_PACK_SIZE) return (USBD_FAIL);
    if (Buf[0] == 0xAA && Buf[1] == 0xBB) {
        // ��������
        if (Buf[2] == 0xCC) {
            CDC_Transmit_FS(&Buf[0], 3);
        }
        // �˵�����
        else if (Buf[2] == 0xDD) {
            set_sign(menu_config, SIGN_SET);
        }
        // �л�д��
        else if (Buf[2] == 0xEE) {
            set_sign(data_state, SIGN_SET);
        }
        // ��ɫͼƬд��
        else if (Buf[2] == 0xFF) {
            set_sign(color_mode, SIGN_SET);
        }
        return (USBD_OK);
    }
    // ���뻺��������
    memcpy((uint8_t *) (&upper_data_p->rec_buff[0] + (MAX_PACK_SIZE * upper_data_p->package_num)), &Buf[0], *Len);
    upper_data_p->package_num++;
    if (*Len < MAX_PACK_SIZE) {
        upper_data_p->package_size += *Len;
        upper_data_p->package_num = 0;
        // ����д�� flash
        set_sign(data_rec, SIGN_SET);
//        CDC_Transmit_FS(&Buf[0], 3);
        return (USBD_OK);
    }
    upper_data_p->package_size += MAX_PACK_SIZE;
    return (USBD_OK);
    /* USER CODE END 6 */
}

/********************************************************************************
* usb ��ʼ��
********************************************************************************/
void usb_init_user(void) {
    sign_data_p = (p_sign_data_t) malloc(sizeof(struct sign_data));
    sign_data_p->sign_byte = 0;
    if (sign_data_p == NULL) {
        free(sign_data_p);
        return;
    }

    usb_data_p = (p_usb_data_t) malloc(sizeof(struct usb_data));
    if (usb_data_p == NULL) {
        free(sign_data_p);
        free(usb_data_p);
        sign_data_p = NULL;
        return;
    }
    usb_data_p->data_buff[0].key_p = key_buff;
    usb_data_p->data_buff[0].zero_p = key_zero;
    usb_data_p->data_buff[0].buff_size = 9;
    usb_data_p->data_buff[1].key_p = mouse_buff;
    usb_data_p->data_buff[1].zero_p = mouse_zero;
    usb_data_p->data_buff[1].buff_size = 5;
    usb_data_p->data_buff[2].key_p = media_buff;
    usb_data_p->data_buff[2].zero_p = media_zero;
    usb_data_p->data_buff[2].buff_size = 5;

    upper_data_p = (p_upper_data_t) malloc(sizeof(struct upper_data));
    if(upper_data_p == NULL) {
        free(sign_data_p);
        free(usb_data_p);
        free(upper_data_p);
        sign_data_p = NULL;
        usb_data_p = NULL;
        return;
    }
    upper_data_p->rec_buff = rec_buff;
}

/********************************************************************************
* ��������
********************************************************************************/
void first_load_menu(void) {
    write_menu = 1;
    // ���ز˵�����
    load_menu_config();
    if (menu_config_arr[0] >= 1 && menu_config_arr[0] <= 10) {
        load_menu(menu_config_arr[0]);
    } else {
        uint8_t *temp = (uint8_t *) calloc(sizeof(uint8_t), 31);
        temp[0] = 1;
        storage_config(temp, 31);
        load_menu(1);
        free(temp);
        temp = NULL;
    }
}


/********************************************************************************
* ���ز˵�
********************************************************************************/
void load_menu(uint8_t menu_index) {
    if (menu_config_arr[menu_index] != 1) return;
    set_menu_lock(1);
    /*// �洢��һ��ʹ�õĲ˵�
    uint8_t cur[1] = {menu_index};
    storage_config(cur, 1);*/
    load_parse_key(menu_index);
    show_menu_oled(menu_index);
    show_menu_color(menu_index);
    set_menu_lock(0);
}

/********************************************************************************
* ���Ļ�������
********************************************************************************/
void set_data_buff(uint8_t func, uint8_t pos, uint8_t data) {
    usb_data_p->data_buff[func].key_p[pos] = data;
}

/********************************************************************************
* ��ȡ�ź���
********************************************************************************/
static uint8_t get_sign(uint8_t shift) {
    return ((sign_data_p->sign_byte >> shift) & 0x01);
}

/********************************************************************************
* �����ź���
********************************************************************************/
static void set_sign(uint8_t shift, uint8_t value) {
    if(value == SIGN_SET) {
        sign_data_p->sign_byte |= (0x01 << shift);
    } else if(value == SIGN_RESET){
        sign_data_p->sign_byte &= ~(0x01 << shift);
    }
}

/********************************************************************************
* USB ɨ��
********************************************************************************/
void usb_scan_user(void) {
    // ��������ջ�������֮��д�� FLASH
    if (get_sign(data_rec) == SIGN_SET) {
        set_sign(data_rec, SIGN_RESET);
        // д��˵�
        if (get_sign(menu_config) == SIGN_SET) {
            set_sign(menu_config, SIGN_RESET);
            storage_config(upper_data_p->rec_buff, 31);
            load_menu_config();
//            lcd_show_str("menu config done");
            receive_reset();
            return;
        }
        // д���ֵ��ͼƬ
        // ����ͼƬ
        if (get_sign(data_state) == SIGN_SET) {
            set_sign(data_state, SIGN_RESET);
            if (upper_data_p->package_size < 5) {
                // ��������
                reset_menu_data(write_menu, 1);
            } else {
                if (upper_data_p->package_size > 730) return;
                // תΪͼƬ���ݺ�洢�� FLASH
                storage_menu_to_flash(write_menu, upper_data_p->rec_buff, upper_data_p->package_size, 1);
            }
            set_sign(key_state, SIGN_RESET);
//            lcd_show_str("oled done");
//            load_menu(write_menu);
            receive_reset();
            return;
        }
        // ����ͼƬ
        if (get_sign(color_mode) == SIGN_SET) {
            // �����ͼƬ���������ڴ�
            if (upper_data_p->package_size < 5) {
                // ��������
                reset_menu_data(write_menu, 2);
                upper_data_p->color_package_count = 0;
                set_sign(color_mode, SIGN_RESET);
//                lcd_show_str("Next ...");
//                load_menu(write_menu);
                write_menu++;
                turn_next_menu();
            } else {
                // �洢ͼƬ
                storage_color_screen(write_menu, upper_data_p->color_package_count << 12, upper_data_p->rec_buff, upper_data_p->package_size);
                // ͼƬ��������ж�
                if (upper_data_p->color_package_count < 26) {
                    upper_data_p->color_package_count++;
                } else {
                    // ͼƬ�������
                    upper_data_p->color_package_count = 0;
                    set_sign(color_mode, SIGN_RESET);
//                    lcd_show_str("Next ...");
//                    load_menu(write_menu);
                    write_menu++;
                    turn_next_menu();
                }
            }
            receive_reset();
            return;
        }
        // ��ʾ��ǰд��Ĳ˵�
        oled_42_show_num(write_menu - 1, 1, 1);
        // д���ֵ
        if (get_sign(key_state) == SIGN_SET) {
            // ��ֵ�洢�ĵڶ���״̬
            set_sign(key_state, SIGN_RESET);
            storage_page_two(write_menu, upper_data_p->rec_buff, upper_data_p->package_size);
        } else {
            if (upper_data_p->package_size < 5) {
                // ��������
                reset_menu_data(write_menu, 0);
            } else {
                // ��ֵ�洢�ĵ�һ��״̬
                set_sign(key_state, SIGN_SET);
                storage_menu_to_flash(write_menu, upper_data_p->rec_buff, upper_data_p->package_size, 0);
            }
        }
//        lcd_show_str("json done");
        receive_reset();
    }
}

/********************************************************************************
* ���ز˵�����
********************************************************************************/
void load_menu_config(void) {
    uint8_t *temp = (uint8_t *) malloc(sizeof(uint8_t) * 31);
    load_config(temp, 31);
    memcpy((uint8_t *) menu_config_arr, (uint8_t *) temp, 31);
//    for(uint8_t i = 0; i < 31; i ++) {
//        printf("temp -> %d", menu_config_arr[i]);
//    }
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
            load_menu(1);
            return;
        }
        write_menu += 1;
    }
}

/********************************************************************************
* ���ñ�־
********************************************************************************/
static void receive_reset(void) {
    upper_data_p->package_size = 0;
    // ��ս��ջ�������
    memset(upper_data_p->rec_buff, 0, RE_BUFF_SIZE);
}

/********************************************************************************
* ���� hid ��
********************************************************************************/
void send_hid_code(uint8_t func) {
    cur_buff = func;
    while (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, usb_data_p->data_buff[func].key_p,
                                      usb_data_p->data_buff[func].buff_size) != USBD_OK);
}

/********************************************************************************
* ���� hid ����
********************************************************************************/
void hid_buff_reset(void) {
    for (uint8_t i = 0; i < usb_data_p->data_buff[cur_buff].buff_size; i++) {
        usb_data_p->data_buff[cur_buff].key_p[i] &= 0x00;
    }
    while (USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, usb_data_p->data_buff[cur_buff].zero_p,
                                      usb_data_p->data_buff[cur_buff].buff_size) != USBD_OK);
//    printf("reset ok\n");
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
