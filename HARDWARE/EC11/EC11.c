#include "EC11.h"
#include "stdio.h"
#include "usart.h"
#include "stm32f1xx_hal.h"

/*
    �˵�ӵ�IO�����ã��������������루�����ź�ʱ�ǵ͵�ƽ����������Ƿ��иߵ�ƽ�ź����룩
    ʹ���ⲿ�ж������EC11�˵��ƽ�仯��
    �жϴ�����ʽ�������ش�������Ҳ������IO�����ó�����������ԭ��
    �жϷ������������жϵĶ˵�Ϊ�ߵ�ƽʱ���жϴ�ʱ��һ�˵��ƽ״̬�Ǹ߻��ǵͣ��Դ����ж���ת������˳ʱ�뻹�� ��ʱ�롣
    �����ת�ٶȴ�С�����Ե����жϷ����������ʱ��
*/


volatile uint16_t angel_count = 0;//�Ƕ��ۼ�
volatile uint8_t encoder_direct_flag = 0;//���ܵ���ת��־��
volatile uint16_t total_angel = once_angel * real_point;//һȦ

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

    // ʹ���ⲿ�ж��ж���
    // ��ռ���ȼ� 2�� �����ȼ� 1
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 1);
    // ʹ���ⲿ�ж�ͨ��
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
//                    printf("EC%d-> ��ʱ��\r\r", i / 2);
                    encoder_callback(i);
                    encoder_direct_flag = 0;
                }
            }else {
                if (GET_ENCODE_STATE(encoder_gpio_array[i - 1]) == HIGH) {
                    encoder_direct_flag = 2;
                }
                if (encoder_direct_flag == 1 && GET_ENCODE_STATE(encoder_gpio_array[i - 1]) == LOW) {
//                    printf("EC%d-> ˳ʱ��\r\r", i / 2);
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
