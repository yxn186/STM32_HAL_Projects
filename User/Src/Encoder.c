/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Encoder.c
  * @brief   旋转编码器
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "Encoder.h"
#include "stm32f1xx_hal_tim.h"
#include "tim.h"
#include <stdint.h>



/**
 * @brief 初始化旋转编码器
 * 
 * @param Encoder_TIM 填写旋转编码器对应的htimx
 */
void Encoder_Init(TIM_HandleTypeDef *Encoder_TIM)
{
    HAL_TIM_Encoder_Start(Encoder_TIM, TIM_CHANNEL_ALL);
}

/**
 * @brief 获取旋转编码器角度
 * 
 * @param Encoder_TIM 填写旋转编码器对应的htimx
 * @return int16_t 返回有符号的角度（-359~+359）
 */
int16_t Encoder_Get_Angle(TIM_HandleTypeDef *Encoder_TIM)
{
    if((int16_t)__HAL_TIM_GET_COUNTER(Encoder_TIM) >= 40 || (int16_t)__HAL_TIM_GET_COUNTER(Encoder_TIM) <= -40)
    {
        __HAL_TIM_SET_COUNTER(Encoder_TIM, 0);
    }
    return __HAL_TIM_GET_COUNTER(Encoder_TIM) * 9;
}