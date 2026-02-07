/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Encoder.h
  * @brief   This file contains all the function prototypes for
  *          the Encoder.c file
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ENCODER_H__
#define __ENCODER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal_tim.h"
/*YOUR CODE*/
/**
 * @brief 初始化旋转编码器
 * 
 * @param Encoder_TIM 填写旋转编码器对应的htimx
 */
void Encoder_Init(TIM_HandleTypeDef *Encoder_TIM);

/**
 * @brief 获取旋转编码器角度
 * 
 * @param Encoder_TIM 填写旋转编码器对应的htimx
 * @return int16_t 返回有符号的角度（-359~+359）
 */
int16_t Encoder_Get_Angle(TIM_HandleTypeDef *Encoder_TIM);



#ifdef __cplusplus
}
#endif

#endif /* __ENCODER_H__ */
