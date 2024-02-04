/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/30 0:04
* @version: 1.0
* @description: 
********************************************************************************/


#include "tim1.h"

TIM_HandleTypeDef htim1;

void tim1_init(void) {
    TIM_Encoder_InitTypeDef sConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 0;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 127;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
    sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
    sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC1Filter = 0;
    sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
    sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC2Filter = 0;
    if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK)
    {
        printf("encode init failed !");
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
    {
        printf("encode init failed !");
    }
    // 开启编码器接口
    HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL); //启动编码器接口
}

/********************************************************************************
* 获取编码器的计数值
********************************************************************************/
uint32_t get_encoder1_counter(void) {
    return (uint16_t ) __HAL_TIM_GET_COUNTER(&htim1);
}

/********************************************************************************
* 设置编码器的计数值
********************************************************************************/
void set_encoder1_counter(uint8_t set_value) {
    __HAL_TIM_SET_COUNTER(&htim1, set_value);
}