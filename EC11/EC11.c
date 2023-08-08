#include "EC11.h"
#include "stdio.h"
#include "usart.h"

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
uint8_t refreshScreen = 1;//刷新屏幕标志位
uint8_t next_menu_flag = 0;


void EC11_EXTI_Init() {
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Pin = EC11_A_PIN | EC11_B_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(EC11_GPIO, &GPIO_InitStruct);

    // 使能外部中断中断线
    // 抢占优先级 2， 子优先级 1
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 1);
    // 使能外部中断通道
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
        // 处理外部中断引脚为GPIO_PIN_0的情况
        if (EC11_B_STATE == HIGH) {
            //可能逆时针
            encoder_direct_flag = 1;
        }
        //第一次和第二次转都是逆时针
        if (encoder_direct_flag == 2 && EC11_B_STATE == LOW) {

            angel_count += once_angel;
            //保证angel_count++之后结果不能大于360度,故先进行判断
            angel_count = angel_count > total_angel ? 0 : angel_count;
            printf("顺时针\n当前度数为：%d\r\n", get_angel());
            encoder_direct_flag = 0;
            refreshScreen = 1;
        }
    } else if (GPIO_Pin == EC11_B_PIN) {
        //B先于A触发下降沿，且A是高电平，第一次可能是顺时针
        if (EC11_A_STATE == HIGH) {
            encoder_direct_flag = 2;
        }
        //第一次和第二次转都是逆时针
        if (encoder_direct_flag == 1 && EC11_A_STATE == LOW) {

            angel_count = angel_count < (once_angel - 1) ? total_angel + once_angel : angel_count;
            angel_count -= once_angel;
            printf("逆时针\n当前度数为：%d\r\n", get_angel());
            encoder_direct_flag = 0;
            refreshScreen = 1;
        }
    }
    // 其他外部中断引脚的处理
}

uint16_t get_angel(void) {

    return angel_count;
}

uint8_t get_refreshScreen(void) {

    return refreshScreen;
}

void flag_to_false(void)//把标志位置为false；
{
    refreshScreen = 0;
}
