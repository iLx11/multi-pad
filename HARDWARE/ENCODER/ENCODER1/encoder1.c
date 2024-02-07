/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/30 8:01
* @version: 1.0
* @description: 
********************************************************************************/


#include "encoder1.h"
#include "tim1.h"

// 初始状态为抬起
static uint8_t encoder_key_state = 1;

void encoder1_init(void) {
    // 定时器初始化
    tim1_init();

    GPIO_InitTypeDef GPIO_InitStruct;
    ENABLE_ENCODER1_KEY_RCC;

    GPIO_InitStruct.Pin = ENCODER1_KEY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ENCODER1_KEY_GPIO, &GPIO_InitStruct);
}

void encoder1_scan(void) {
    // 编码器按键扫描
    encoder1_key_scan();
    // 编码器旋转扫描
    encoder1_rotation_scan();
}

/********************************************************************************
* 编码器旋转扫描
********************************************************************************/
static void encoder1_rotation_scan(void) {
    uint16_t encoder_counter = get_encoder1_counter();
    if(encoder_counter % 4 == 0 && encoder_counter != 0) {
        if(encoder_counter == 4) {
            if(GET_ENCODER1_KEY_STATE == ENCODER1_KEY_DOWN) {
                // 按住左旋回调
                encoder1_callback(DOWN_ANTICLOCKWISE);
//                encoder1_hold_a_callback();
                set_encoder1_counter(0);
                return;
            }
            // 编码器逆时针回调
            encoder1_callback(ANTICLOCKWISE);
//            encoder1_anticlockwise_callback();
        }else if(encoder_counter == 124) {
            if(GET_ENCODER1_KEY_STATE == ENCODER1_KEY_DOWN) {
                // 按住右旋回调
                encoder1_callback(UP_CLOCKWISE);
//                encoder1_hold_c_callback();
                set_encoder1_counter(0);
                return;
            }
            // 编码器顺时针回调
            encoder1_callback(0);
//            encoder1_clockwise_callback();
        }
        set_encoder1_counter(CLOCKWISE);
    }
}

/********************************************************************************
* 编码器按键扫描
********************************************************************************/
static void encoder1_key_scan(void) {
    if(GET_ENCODER1_KEY_STATE == ENCODER1_KEY_DOWN  && encoder_key_state == 1) {
        encoder_key_state = ENCODER1_KEY_DOWN;
        // 编码器按下回调
        encoder1_callback(ENCODER_DOWN);
//        encoder1_key_down_callback();
    }else if(GET_ENCODER1_KEY_STATE == ENCODER1_KEY_UP && encoder_key_state == 0) {
        encoder_key_state = ENCODER1_KEY_UP;
        // 编码器抬起回调
        encoder1_callback(ENCODER_UP);
//        encoder1_key_up_callback();
    }
}