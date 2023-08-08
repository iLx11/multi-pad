#ifndef EC11_H

#define EC11_H
#include  "stm32f1xx.h"
#include "system_stm32f1xx.h"
#include "oled.h"

#define  once_angel         18//1����λ��ת�Ķ���
#define  point              20//һȦ��20����λ��
#define  real_point	      5  //ʵ��ת������λ������¼�
#define EC11_GPIO_RCC 		RCC_APB2Periph_GPIOC

#define EC11_GPIO  				GPIOC
#define EC11_A_PIN  			GPIO_PIN_6
#define EC11_B_PIN  			GPIO_PIN_1

#define EC11_EXTI_A_PIN     GPIO_PinSource6

#define EC11_EXTI_B_PIN      GPIO_PinSource1

#define EC11_A_EXTI_Line    EXTI_Line6         //�ж���ѡ��
#define EC11_B_EXTI_Line    EXTI_Line1         //�ж���ѡ��
#define EXTI_A_EXTI_Mode    EXTI_Trigger_Falling //�½��ش���
#define EXTI_B_EXTI_Mode    EXTI_Trigger_Falling//�½��ش���

#define HIGH              1
#define LOW               0

#define CW                1//˳ʱ��
#define CWW               0//��ʱ��


#define EC11_A_STATE      HAL_GPIO_ReadPin(EC11_GPIO,EC11_A_PIN)
#define EC11_B_STATE      HAL_GPIO_ReadPin(EC11_GPIO,EC11_B_PIN)

void EC11_EXTI_Init();
uint16_t get_angel(void);
void EXTI1_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
uint8_t get_refreshScreen(void); // �Ƿ���Ҫˢ����Ļ

void flag_to_false(void);//��ˢ�±�־λ��Ϊ0

#endif