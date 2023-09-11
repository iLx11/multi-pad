#include "EC11.h"
#include "stdio.h"
#include "usart.h"
#include "stm32f1xx_hal.h"

/*
    端点接的IO口设置：均采用上拉输入（不接信号时是低电平，用来检测是否有高电平信号输入）
    使用外部中断来检测EC11端点电平变化：
    中断触发方式：上升沿触发（这也是上面IO口设置成下拉输出入的原因）
    中断服务函数：触发中断的端点为高电平时，判断此时另一端点电平状态是高还是低，以此来判断旋转方向是顺时针还是 逆时针。
    针对旋转速度大小，可以调节中断服务函数里的延时。
*/


volatile uint16_t angel_count = 0;//角度累加
volatile uint8_t encoder_direct_flag = 0;//可能的旋转标志。
volatile uint16_t total_angel = once_angel * real_point;//一圈

void EC11_EXTI_Init() {
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Pin = EC11_A0_PIN | EC11_B0_PIN |
                          EC11_A1_PIN | EC11_B1_PIN |
                          EC11_A2_PIN | EC11_B2_PIN |
                          EC11_A3_PIN | EC11_B3_PIN |
                          EC11_A4_PIN | EC11_B4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(EC11_GPIO, &GPIO_InitStruct);

    // 使能外部中断中断线
    // 抢占优先级 2， 子优先级 1
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 1);
    // 使能外部中断通道
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

uint16_t encoder_gpio_array[10] = {
        EC11_A2_PIN, EC11_B2_PIN,
        EC11_A1_PIN, EC11_B1_PIN,
        EC11_A0_PIN, EC11_B0_PIN,
        EC11_A3_PIN, EC11_B3_PIN,
        EC11_A4_PIN, EC11_B4_PIN,
};

void set_encoder_state(uint16_t GPIO_Pin) {
    for (uint8_t i = 0; i < 10; i++) {
        if (GPIO_Pin == encoder_gpio_array[i]) {
            if (i % 2 == 0) {
                if (GET_ENCODE_STATE(encoder_gpio_array[i + 1]) == HIGH) {
                    encoder_direct_flag = 1;
                }
                if (encoder_direct_flag == 2 && GET_ENCODE_STATE(encoder_gpio_array[i + 1]) == LOW) {
//                    printf("EC%d-> 逆时针\r\r", i / 2);
                    encoder_callback(i);
                    encoder_direct_flag = 0;
                }
            }else {
                if (GET_ENCODE_STATE(encoder_gpio_array[i - 1]) == HIGH) {
                    encoder_direct_flag = 2;
                }
                if (encoder_direct_flag == 1 && GET_ENCODE_STATE(encoder_gpio_array[i - 1]) == LOW) {
//                    printf("EC%d-> 顺时针\r\r", i / 2);
                    encoder_callback(i);
                    encoder_direct_flag = 0;
                }
            }
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    set_encoder_state(GPIO_Pin);
}
