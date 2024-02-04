/********************************************************************************
* @author: iLx1
* @email: colorful_ilx1@163.com
* @date: 2023/11/30 0:04
* @version: 1.0
* @description: 
********************************************************************************/


#include "tim4.h"

TIM_HandleTypeDef htim4;

void tim4_init(void) {
    TIM_Encoder_InitTypeDef sConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 0;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 127;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
    sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
    sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC1Filter = 12;
    sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
    sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC2Filter = 12;
    if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
    {
        printf("encode init failed !");
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
    {
        printf("encode init failed !");
    }
    // 开启编码器接口
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL); //启动编码器接口
}

/********************************************************************************
* 获取编码器的计数值
********************************************************************************/
uint32_t get_encoder3_counter(void) {
    return (uint16_t ) __HAL_TIM_GET_COUNTER(&htim4);
}

/********************************************************************************
* 设置编码器的计数值
********************************************************************************/
void set_encoder3_counter(uint8_t set_value) {
    __HAL_TIM_SET_COUNTER(&htim4, set_value);
}

/********************************************************************************
* 定时器底层初始化
********************************************************************************/
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* tim_encoderHandle) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(tim_encoderHandle->Instance==TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_GPIOE_CLK_ENABLE();
        /**TIM4 GPIO Configuration
        PD12     ------> TIM4_CH1
        PD13     ------> TIM4_CH2
        */
        GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
        __HAL_AFIO_REMAP_TIM1_ENABLE();
    }

    if(tim_encoderHandle->Instance==TIM4) {
        __HAL_RCC_TIM4_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        /**TIM4 GPIO Configuration
        PD12     ------> TIM4_CH1
        PD13     ------> TIM4_CH2
        */
        GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
        __HAL_AFIO_REMAP_TIM4_ENABLE();
    }

    if(tim_encoderHandle->Instance==TIM3) {
        __HAL_RCC_TIM3_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        /**TIM4 GPIO Configuration
        PC6     ------> TIM4_CH1
        PC7     ------> TIM4_CH2
        */
        GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        __HAL_AFIO_REMAP_TIM3_ENABLE();
    }
}