/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    servo.c
  * @brief   舵机使用 PWM要调成50Hz！！！
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "servo.h"
#include "stm32f1xx_hal_tim.h"
#include "tim.h"
#include <stdint.h>

/**
 * @brief ARR值
 * 
 */
#define TIM_ARR 2000

/**
 * @brief 初始化舵机
 * 
 * @param Servo_TIM 舵机htimx
 * @param Servo_TIM_Channel 舵机TIM通道（TIM_CHANNEL_1/2/3/4）
 */
void Servo_Init(TIM_HandleTypeDef *Servo_TIM,uint32_t Servo_TIM_Channel)
{
    HAL_TIM_PWM_Start(Servo_TIM, Servo_TIM_Channel);
}

/**
 * @brief 设置舵机角度函数
 * 
 * @param Servo_TIM 舵机htimx
 * @param Servo_TIM_Channel 舵机TIM通道（TIM_CHANNEL_1/2/3/4）
 * @param Angle 需要设置的角度（-90~90） 若超过范围则会维持在边界
 * @return int16_t 返回占空比
 */
int16_t Servo_Set_Angle(TIM_HandleTypeDef *Servo_TIM,uint32_t Servo_TIM_Channel,int16_t Angle)
{
  if(Angle >= 90)
  {
    Angle = 90;
  }
  else if(Angle <= -90)
  {
    Angle = -90;
  }
  //输入的是-90~90 + 90 变成是0-180
  // 将角度-90~90 映射到占空比 2.5%~12.5%
  float Duty_Percent = ((float)(Angle + 90) / 180.0 * (12.5 - 2.5)) + 2.5;  
  // 将占空比转换为 PWM 信号对应的计数值
  int16_t Duty = (Duty_Percent / 100.0) * TIM_ARR;  // 对应于ARR 2000时的Duty值

  __HAL_TIM_SET_COMPARE(Servo_TIM, Servo_TIM_Channel, Duty);
  return Duty;
}