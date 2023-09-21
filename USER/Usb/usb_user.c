#include "usb_user.h"
#include "oled_user.h"
#include "flash_user.h"
#include "usbd_customhid.h"
#include "jsmn_user.h"

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

// ͼƬ����
extern uint8_t oled_42_array[OLED_42_NUM][SIZE_42];
extern uint8_t oled_96_array[OLED_96_NUM][SIZE_96];

// ��������
extern char json_str[JSON_SIZE];
char clear_str[JSON_SIZE] = {0};

uint8_t folder_index = 0;
uint8_t file_array_index = 0;
uint16_t file_position = 0;
uint8_t photo_file_flag = 0;

static void photo_string_to_hex(const char *hex_string_array);

static void turn_to_next_position();

static void set_key_value(const char *string_key_setting);

static uint8_t string_to_num_hex(const char *hex_string_array, uint8_t start, uint8_t end);

static uint8_t char_to_hex(char hex_char);


/********************************************************************************
* ���� hid ��λ�����͵�����
********************************************************************************/
void receive_data_from_upper(USBD_CUSTOM_HID_HandleTypeDef *hid_handle, uint8_t len) {
    if (hid_handle->Report_buf[0] == '!' && hid_handle->Report_buf[1] == '@' && hid_handle->Report_buf[2] == '$') {
//        USBD_CUSTOM_HID_SendReport();
        return ;
    }
    // �����ļ���д��
    // ͼƬд��
    if (file_array_index < (OLED_42_NUM + OLED_96_NUM)) {
        char hex_string_array[129];
        for (uint8_t i = 0; i < len; i++) {
            sprintf(hex_string_array + (i * 2), "%02x", hid_handle->Report_buf[i]);
        }
        // ��16���Ƶ��ַ���ͼƬ����תΪ��ֵ��д��ͼƬ����
        photo_string_to_hex(hex_string_array);
        // ת����һ��ͼƬ��λ��
        turn_to_next_position();

    } else {
        // ����ͼƬ����
        if (file_array_index == (OLED_42_NUM + OLED_96_NUM) && photo_file_flag == 0) {
            photo_file_flag = 1;
            printf("photo_array_done\n\r");
            // ��һ���㼶��ͼƬ���� flash
            menu_photo_folder_storage(folder_index);
            // ����ַ���
            memset(json_str, 0, JSON_SIZE);
        }
/*        printf("hid_handle->Report_buf[0] -> %c, %d\n\r", hid_handle->Report_buf[0], hid_handle->Report_buf[0]);
        printf("hid_handle->Report_buf[1] -> %c, %d\n\r", hid_handle->Report_buf[1], hid_handle->Report_buf[1]);
        printf("hid_handle->Report_buf[2] -> %c, %d\n\r", hid_handle->Report_buf[2], hid_handle->Report_buf[2]);*/
        // ��ֵ�����
        if ((char) hid_handle->Report_buf[0] == '#' && (char) hid_handle->Report_buf[1] == '$' &&
            (char) hid_handle->Report_buf[2] == '%') {
            printf("key_done\n\r");
            printf("json_str -> %s\n\r", json_str);
            // ��ֵд�� flash
            storage_setting_to_flash(folder_index, clear_str, JSON_SIZE);
            storage_setting_to_flash(folder_index, json_str, JSON_SIZE);
            // ����ַ���
            memset(json_str, 0, JSON_SIZE);
            file_array_index = 0;
            file_position = 0;
            photo_file_flag = 0;
            folder_index > 9 ? folder_index = 0 : folder_index ++;
        } else {
            strncat(json_str, (char *) hid_handle->Report_buf, 64);
            file_position += 64;
        }
    }
}

/********************************************************************************
* ���ü�ֵ
********************************************************************************/
static void set_key_value(const char *string_key_setting) {

}

/********************************************************************************
* ��16���Ƶ��ַ���ͼƬ����תΪ��ֵ��д��ͼƬ����
********************************************************************************/
static void photo_string_to_hex(const char *hex_string_array) {
    for (uint8_t i = 0; i < 128;) {
        if (file_array_index < OLED_42_NUM && file_position >= 360) break;
        if (file_array_index >= OLED_42_NUM && file_position >= 999) break;
        if (file_array_index < OLED_42_NUM)
            oled_42_array[file_array_index][file_position++] = string_to_num_hex(hex_string_array, i++, i++);
        else if (file_array_index >= OLED_42_NUM) {
            oled_96_array[file_array_index - OLED_42_NUM][file_position++] = string_to_num_hex(hex_string_array,
                                                                                               i++, i++);
        }
    }
}

/********************************************************************************
* ת����һ��ͼƬ��λ��
********************************************************************************/
static void turn_to_next_position() {
    if (file_array_index < OLED_42_NUM && file_position >= 360) {
        /*OLED_42_ShowPicture(oled_42_x, oled_42_y, oled_42_l, oled_42_h,
                            oled_42_array[file_array_index], 1, file_array_index);*/
        // һ��ͼƬ����
        file_position = 0;
        // ��һ��ͼƬ
        file_array_index += 1;
    } else if (file_array_index >= OLED_42_NUM && file_position >= 999) {
        /*OLED_92_ShowPicture(oled_96_x, oled_96_y, oled_96_l, oled_96_h,
                            oled_96_array[file_array_index - OLED_42_NUM], 1, file_array_index - OLED_42_NUM);*/
        file_position = 0;
        file_array_index += 1;
    }
}


/********************************************************************************
* ���� hid ��
********************************************************************************/
void send_hid_code(uint8_t func) {
    cur_buff = func;
    while (USBD_CUSTOM_HID_FUNC_SendReport(&hUsbDeviceFS, buff_point_array[func].send_buff_point,
                                           buff_point_array[func].buff_size) != USBD_OK);
}

/********************************************************************************
* ���� hid ����
********************************************************************************/
void hid_buff_reset() {
    for (uint8_t i = 0; i < buff_point_array[cur_buff].buff_size; i++) {
        buff_point_array[cur_buff].send_buff_point[i] &= 0x00;
    }
    while (USBD_CUSTOM_HID_FUNC_SendReport(&hUsbDeviceFS, buff_point_array[cur_buff].zero_buff_point,
                                           buff_point_array[cur_buff].buff_size) != USBD_OK);
    printf("reset ok\n");
}

/********************************************************************************
* �ַ�תʮ������
********************************************************************************/
static uint8_t string_to_num_hex(const char *hex_string_array, uint8_t start, uint8_t end) {
    return char_to_hex(hex_string_array[start]) * 16 + char_to_hex(hex_string_array[end]);
}

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
