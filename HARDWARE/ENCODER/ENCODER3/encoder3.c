/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/30 8:01
* @version: 1.0
* @description: 
********************************************************************************/


#include "encoder3.h"
#include "tim4.h"
#include "encoder1.h"

// ��ʼ״̬Ϊ̧��
static uint8_t encoder_key_state = 1;

void encoder3_init(void) {
    // ��ʱ����ʼ��
    tim4_init();

    GPIO_InitTypeDef GPIO_InitStruct;
    ENABLE_ENCODER3_KEY_RCC;

    GPIO_InitStruct.Pin = ENCODER3_KEY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ENCODER3_KEY_GPIO, &GPIO_InitStruct);
}

void encoder3_scan(void) {
    // ����������ɨ��
    encoder3_key_scan();
    // ��������תɨ��
    encoder3_rotation_scan();
}

/********************************************************************************
* ��������תɨ��
********************************************************************************/
static void encoder3_rotation_scan(void) {
    uint16_t encoder_counter = get_encoder3_counter();
    if(encoder_counter % 4 == 0 && encoder_counter != 0) {
        if(encoder_counter == 4) {
            if(GET_ENCODER3_KEY_STATE == ENCODER3_KEY_DOWN) {
                // ��ס�����ص�
                encoder3_callback(DOWN_ANTICLOCKWISE);
//                encoder3_hold_a_callback();
                set_encoder3_counter(0);
                return;
            }
            // ��������ʱ��ص�
            encoder3_callback(ANTICLOCKWISE);
//            encoder3_anticlockwise_callback();
        }else if(encoder_counter == 124) {
            if(GET_ENCODER3_KEY_STATE == ENCODER3_KEY_DOWN) {
                // ��ס�����ص�
                encoder3_callback(UP_CLOCKWISE);
//                encoder3_hold_c_callback();
                set_encoder3_counter(0);
                return;
            }
            // ������˳ʱ��ص�
            encoder3_callback(CLOCKWISE);
//            encoder3_clockwise_callback();
        }
        set_encoder3_counter(0);
    }
}

/********************************************************************************
* ����������ɨ��
********************************************************************************/
static void encoder3_key_scan(void) {
    if(GET_ENCODER3_KEY_STATE == ENCODER3_KEY_DOWN  && encoder_key_state == 1) {
        encoder_key_state = ENCODER3_KEY_DOWN;
        // ���������»ص�
        encoder3_callback(ENCODER_DOWN);
//        encoder3_key_down_callback();
    }else if(GET_ENCODER3_KEY_STATE == ENCODER3_KEY_UP && encoder_key_state == 0) {
        encoder_key_state = ENCODER3_KEY_UP;
        // ������̧��ص�
        encoder3_callback(ENCODER_UP);
//        encoder3_key_up_callback();
    }
}