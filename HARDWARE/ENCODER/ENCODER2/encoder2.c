/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/30 8:01
* @version: 1.0
* @description:
********************************************************************************/


#include "encoder2.h"
#include "tim3.h"
#include "encoder1.h"

// ��ʼ״̬Ϊ̧��
static uint8_t encoder_key_state = 1;

void encoder2_init(void) {
    // ��ʱ����ʼ��
    tim3_init();

    GPIO_InitTypeDef GPIO_InitStruct;
    ENABLE_ENCODER2_KEY_RCC;

    GPIO_InitStruct.Pin = ENCODER2_KEY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ENCODER2_KEY_GPIO, &GPIO_InitStruct);
}

void encoder2_scan(void) {
    // ����������ɨ��
    encoder2_key_scan();
    // ��������תɨ��
    encoder2_rotation_scan();
}

/********************************************************************************
* ��������תɨ��
********************************************************************************/
static void encoder2_rotation_scan(void) {
    uint16_t encoder_counter = get_encoder2_counter();
    if(encoder_counter % 4 == 0 && encoder_counter != 0) {
        if(encoder_counter == 4) {
            if(GET_ENCODER2_KEY_STATE == ENCODER2_KEY_DOWN) {
                // ��ס�����ص�
                encoder2_callback(DOWN_ANTICLOCKWISE);
//                encoder2_hold_a_callback();
                set_encoder2_counter(0);
                return;
            }
            // ��������ʱ��ص�
            encoder2_callback(ANTICLOCKWISE);
//            encoder2_anticlockwise_callback();
        }else if(encoder_counter == 124) {
            if(GET_ENCODER2_KEY_STATE == ENCODER2_KEY_DOWN) {
                // ��ס�����ص�
                encoder2_callback(UP_CLOCKWISE);
//                encoder2_hold_c_callback();
                set_encoder2_counter(0);
                return;
            }
            // ������˳ʱ��ص�
            encoder2_callback(CLOCKWISE);
//            encoder2_clockwise_callback();
        }
        set_encoder2_counter(0);
    }
}

/********************************************************************************
* ����������ɨ��
********************************************************************************/
static void encoder2_key_scan(void) {
    if(GET_ENCODER2_KEY_STATE == ENCODER2_KEY_DOWN  && encoder_key_state == 1) {
        encoder_key_state = ENCODER2_KEY_DOWN;
        // ���������»ص�
        encoder2_callback(ENCODER_DOWN);
//        encoder2_key_down_callback();
    }else if(GET_ENCODER2_KEY_STATE == ENCODER2_KEY_UP && encoder_key_state == 0) {
        encoder_key_state = ENCODER2_KEY_UP;
        // ������̧��ص�
        encoder2_callback(ENCODER_UP);
//        encoder2_key_up_callback();
    }
}