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


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    printf("GPIO_PIN -> %d\n", GPIO_Pin);
    if (GPIO_Pin == EC11_A0_PIN) {
        printf("EC11_B0_STATE -> %d\n", EC11_B0_STATE);

        // �����ⲿ�ж�����ΪGPIO_PIN_0�����
        if (EC11_B0_STATE == HIGH) {
            //������ʱ��
            encoder_direct_flag = 1;
        }
        //��һ�κ͵ڶ���ת������ʱ��
        if (encoder_direct_flag == 2 && EC11_B0_STATE == LOW) {
            angel_count += once_angel;
            //��֤angel_count++֮�������ܴ���360��,���Ƚ����ж�
            angel_count = angel_count > total_angel ? 0 : angel_count;
            printf("EC0-> ˳ʱ��\n��ǰ����Ϊ��%d\r\n", get_angel());
            encoder_direct_flag = 0;
        }
    }
    if (GPIO_Pin == EC11_B0_PIN) {
        printf("EC11_B0_STATE -> %d\n", EC11_B0_STATE);

        //B����A�����½��أ���A�Ǹߵ�ƽ����һ�ο�����˳ʱ��
        if (EC11_A0_STATE == HIGH) {
            encoder_direct_flag = 2;
        }
        //��һ�κ͵ڶ���ת������ʱ��
        if (encoder_direct_flag == 1 && EC11_A0_STATE == LOW) {
            angel_count = angel_count < (once_angel - 1) ? total_angel + once_angel : angel_count;
            angel_count -= once_angel;
            printf("EC0-> ��ʱ��\n��ǰ����Ϊ��%d\r\n", get_angel());
            encoder_direct_flag = 0;
        }
    }

    if (GPIO_Pin == 32) {
        printf("EC11_B1_STATE -> %d\n", EC11_B1_STATE);
        // �����ⲿ�ж�����ΪGPIO_PIN_0�����
        if (EC11_B1_STATE == HIGH) {

            //������ʱ��
            encoder_direct_flag = 1;
        }
        //��һ�κ͵ڶ���ת������ʱ��
        if (encoder_direct_flag == 2 && EC11_B1_STATE == LOW) {
            printf("EC11_B1_STATE low\n");
            angel_count += once_angel;
            //��֤angel_count++֮�������ܴ���360��,���Ƚ����ж�
            angel_count = angel_count > total_angel ? 0 : angel_count;
            printf("EC1-> ˳ʱ��\n��ǰ����Ϊ��%d\r\n", get_angel());
            encoder_direct_flag = 0;
        }
    }
    if (GPIO_Pin == 64) {
        printf("EC11_B1_STATE -> %d\n", EC11_B1_STATE);
        //B����A�����½��أ���A�Ǹߵ�ƽ����һ�ο�����˳ʱ��
        if (EC11_A1_STATE == HIGH) {
            encoder_direct_flag = 2;
        }
        //��һ�κ͵ڶ���ת������ʱ��
        if (encoder_direct_flag == 1 && EC11_A1_STATE == LOW) {
            angel_count = angel_count < (once_angel - 1) ? total_angel + once_angel : angel_count;
            angel_count -= once_angel;
            printf("EC1-> ��ʱ��\n��ǰ����Ϊ��%d\r\n", get_angel());
            encoder_direct_flag = 0;
        }
    }

    if (GPIO_Pin == EC11_A2_PIN) {
        // �����ⲿ�ж�����ΪGPIO_PIN_0�����
        if (EC11_B2_STATE == HIGH) {
            //������ʱ��
            encoder_direct_flag = 1;
        }
        //��һ�κ͵ڶ���ת������ʱ��
        if (encoder_direct_flag == 2 && EC11_B2_STATE == LOW) {
            angel_count += once_angel;
            //��֤angel_count++֮�������ܴ���360��,���Ƚ����ж�
            angel_count = angel_count > total_angel ? 0 : angel_count;
            printf("EC2-> ˳ʱ��\n��ǰ����Ϊ��%d\r\n", get_angel());
            encoder_direct_flag = 0;
        }
    }
    if (GPIO_Pin == EC11_B2_PIN) {
        //B����A�����½��أ���A�Ǹߵ�ƽ����һ�ο�����˳ʱ��
        if (EC11_A2_STATE == HIGH) {
            encoder_direct_flag = 2;
        }
        //��һ�κ͵ڶ���ת������ʱ��
        if (encoder_direct_flag == 1 && EC11_A2_STATE == LOW) {
            angel_count = angel_count < (once_angel - 1) ? total_angel + once_angel : angel_count;
            angel_count -= once_angel;
            printf("EC2-> ��ʱ��\n��ǰ����Ϊ��%d\r\n", get_angel());
            encoder_direct_flag = 0;
        }
    }

    if (GPIO_Pin == EC11_A3_PIN) {
        // �����ⲿ�ж�����ΪGPIO_PIN_0�����
        if (EC11_B3_STATE == HIGH) {
            //������ʱ��
            encoder_direct_flag = 1;
        }
        //��һ�κ͵ڶ���ת������ʱ��
        if (encoder_direct_flag == 2 && EC11_B3_STATE == LOW) {
            angel_count += once_angel;
            //��֤angel_count++֮�������ܴ���360��,���Ƚ����ж�
            angel_count = angel_count > total_angel ? 0 : angel_count;
            printf("EC3-> ˳ʱ��\n��ǰ����Ϊ��%d\r\n", get_angel());
            encoder_direct_flag = 0;
        }
    }
    if (GPIO_Pin == EC11_B3_PIN) {
        //B����A�����½��أ���A�Ǹߵ�ƽ����һ�ο�����˳ʱ��
        if (EC11_A3_STATE == HIGH) {
            encoder_direct_flag = 2;
        }
        //��һ�κ͵ڶ���ת������ʱ��
        if (encoder_direct_flag == 1 && EC11_A3_STATE == LOW) {
            angel_count = angel_count < (once_angel - 1) ? total_angel + once_angel : angel_count;
            angel_count -= once_angel;
            printf("EC3-> ��ʱ��\n��ǰ����Ϊ��%d\r\n", get_angel());
            encoder_direct_flag = 0;
        }
    }

    if (GPIO_Pin == EC11_A4_PIN) {
        // �����ⲿ�ж�����ΪGPIO_PIN_0�����
        if (EC11_B4_STATE == HIGH) {
            //������ʱ��
            encoder_direct_flag = 1;
        }
        //��һ�κ͵ڶ���ת������ʱ��
        if (encoder_direct_flag == 2 && EC11_B4_STATE == LOW) {
            angel_count += once_angel;
            //��֤angel_count++֮�������ܴ���360��,���Ƚ����ж�
            angel_count = angel_count > total_angel ? 0 : angel_count;
            printf("EC4-> ˳ʱ��\n��ǰ����Ϊ��%d\r\n", get_angel());
            encoder_direct_flag = 0;
        }
    }
    if (GPIO_Pin == EC11_B4_PIN) {
        //B����A�����½��أ���A�Ǹߵ�ƽ����һ�ο�����˳ʱ��
        if (EC11_A4_STATE == HIGH) {
            encoder_direct_flag = 2;
        }
        //��һ�κ͵ڶ���ת������ʱ��
        if (encoder_direct_flag == 1 && EC11_A4_STATE == LOW) {
            angel_count = angel_count < (once_angel - 1) ? total_angel + once_angel : angel_count;
            angel_count -= once_angel;
            printf("EC4-> ��ʱ��\n��ǰ����Ϊ��%d\r\n", get_angel());
            encoder_direct_flag = 0;
        }
    }
}


uint16_t get_angel(void) {
    return angel_count;
}
