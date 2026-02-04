/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    key.h
  * @brief   This file contains all the function prototypes for
  *          the key.c file
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KEY_H__
#define __KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/*YOUR CODE*/
#define KEY_TOTAL_NUM 1

/**
 * @brief 枚举按钮按下/没按下
 * 
 */
typedef enum{
    KEY_PRESSED,
    KEY_UNPRESSED
} Key_Pressed_State;

/**
 * @brief 枚举按钮标志位
 * 
 */
typedef enum{
    KEY_Flag_FREE,      //按键空闲
    KEY_Flag_DOWN,      //按键按下
    KEY_Flag_UP,        //按键单击松开
    KEY_Flag_DOUBLE,    //按键双击
    KEY_Flag_LONG,      //按键长按
    KEY_Flag_REPEAT,    //按键重复
    KEY_Flag_ERROR      //按键错误
} Key_Flag;

/**
 * @brief 枚举按钮状态
 * 
 */
typedef enum{
    KEY_FREE,
    KEY_SINGLE,
    KEY_DOUBLE,
    KEY_LONG,
    KEY_REPEAT,
    KEY_ERROR
} Key_State;

/**
 * @brief 检查特定按钮是否按下
 * 
 * @param Keynum 按钮编号
 * @return Key_Pressed_State KEY_PRESSED/KEY_UNPRESSED
 */
Key_Pressed_State Key_Check_PRESSED(uint8_t Keynum);

/**
 * @brief 获取当前特定按钮状态函数
 * 
 * @param KeyNum 按钮编号
 * @return Key_State KEY_FREE,KEY_SINGLE,KEY_DOUBLE,KEY_LONG,KEY_ERROR
 */
Key_Flag Key_Get_Current_State(uint8_t KeyNum);

/**
 * @brief 获取当前特定按钮事件函数
 * 
 * @param KeyNum 按钮编号
 * @return Key_State 
 */
Key_State Key_Get_Event(uint8_t KeyNum);

/**
 * @brief 放入定时器中断 1ms执行一次 按键定时检测函数
 * 
 */
void Key_Tick(void);


#ifdef __cplusplus
}
#endif

#endif /* __KEY_H__ */
