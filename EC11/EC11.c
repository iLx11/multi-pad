#include "EC11.h"
#include "stdio.h"
#include "usart.h"

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
uint8_t refreshScreen = 1;//ˢ����Ļ��־λ
uint8_t next_menu_flag = 0;


void EC11_EXTI_Init() {
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Pin = EC11_A_PIN | EC11_B_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(EC11_GPIO, &GPIO_InitStruct);

    // ʹ���ⲿ�ж��ж���
    // ��ռ���ȼ� 2�� �����ȼ� 1
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 1);
    // ʹ���ⲿ�ж�ͨ��
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);

}

/*void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_PIN) {
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN);
        HAL_GPIO_EXTI_Callback(GPIO_PIN);
    }
}*/

void EXTI1_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(EC11_A_PIN);
}

void EXTI9_5_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(EC11_B_PIN);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    printf("saadsffasdfas");
    if (GPIO_Pin == EC11_A_PIN) {
        // �����ⲿ�ж�����ΪGPIO_PIN_0�����
        if (EC11_B_STATE == HIGH) {
            //������ʱ��
            encoder_direct_flag = 1;
        }
        //��һ�κ͵ڶ���ת������ʱ��
        if (encoder_direct_flag == 2 && EC11_B_STATE == LOW) {

            angel_count += once_angel;
            //��֤angel_count++֮�������ܴ���360��,���Ƚ����ж�
            angel_count = angel_count > total_angel ? 0 : angel_count;
            printf("˳ʱ��\n��ǰ����Ϊ��%d\r\n", get_angel());
            encoder_direct_flag = 0;
            refreshScreen = 1;
        }
    } else if (GPIO_Pin == EC11_B_PIN) {
        //B����A�����½��أ���A�Ǹߵ�ƽ����һ�ο�����˳ʱ��
        if (EC11_A_STATE == HIGH) {
            encoder_direct_flag = 2;
        }
        //��һ�κ͵ڶ���ת������ʱ��
        if (encoder_direct_flag == 1 && EC11_A_STATE == LOW) {

            angel_count = angel_count < (once_angel - 1) ? total_angel + once_angel : angel_count;
            angel_count -= once_angel;
            printf("��ʱ��\n��ǰ����Ϊ��%d\r\n", get_angel());
            encoder_direct_flag = 0;
            refreshScreen = 1;
        }
    }
    // �����ⲿ�ж����ŵĴ���
}

uint16_t get_angel(void) {

    return angel_count;
}

uint8_t get_refreshScreen(void) {

    return refreshScreen;
}

void flag_to_false(void)//�ѱ�־λ��Ϊfalse��
{
    refreshScreen = 0;
}
