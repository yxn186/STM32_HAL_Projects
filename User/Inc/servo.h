/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    servo.h
  * @brief   This file contains all the function prototypes for
  *          the servo.c file
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SERVO_H__
#define __SERVO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal_tim.h"
/*YOUR CODE*/

/**
 * @brief 初始化舵机
 * 
 */
void Servo_Init(TIM_HandleTypeDef *Servo_TIM,uint32_t Servo_TIM_Channel);

/**
 * @brief 设置舵机角度函数
 * 
 * @param Servo_TIM 舵机htimx
 * @param Servo_TIM_Channel 舵机TIM通道（TIM_CHANNEL_1/2/3/4）
 * @param Angle 需要设置的角度（-90~90） 若超过范围则会维持在边界
 * @return int16_t 返回占空比
 */
int16_t Servo_Set_Angle(TIM_HandleTypeDef *Servo_TIM,uint32_t Servo_TIM_Channel,int16_t Angle);


#ifdef __cplusplus
}
#endif

#endif /* __SERVO_H__ */
