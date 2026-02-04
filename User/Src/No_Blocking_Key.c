/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    key.c
  * @brief   全功能非阻塞按键 需要配合定时器使用 1ms刷新一次key_tick!!
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "No_Blocking_Key.h"
#include "stm32f1xx_hal_gpio.h"
#include <stdint.h>

//按钮设置
#define KEY1_GPIO GPIOB
#define KEY1_GPIO_PIN GPIO_PIN_11

//各时间阈值 
#define KEY_TIME_DOUBLE 200	//双击阈值（第一次按下按键后 隔多久才算双击）
#define KEY_TIME_LONG 1000		//长按阈值（按下不放多久才算长按）
#define KEY_TIME_REPEAT 200	//重复阈值（长按后每隔多久REPEAT置一次1）数值不能无限改小 不会超过主循环刷新速度
//因为按键检测和状态机都是20ms执行一次 所以下面最好都设为20ms的倍数


Key_Flag Key_Current_Flag[KEY_TOTAL_NUM] = {KEY_Flag_FREE};
Key_State Key_Current_State[KEY_TOTAL_NUM] = {KEY_FREE};

/**
 * @brief 检查特定按钮是否按下
 * 
 * @param Keynum 按钮编号
 * @return Key_Pressed_State：KEY_PRESSED/KEY_UNPRESSED
 */
Key_Pressed_State Key_Check_PRESSED(uint8_t KeyNum)
{
    if(KeyNum == 1)
    {
        if(HAL_GPIO_ReadPin(KEY1_GPIO, KEY1_GPIO_PIN) == GPIO_PIN_RESET)
        {
            return KEY_PRESSED;
        }
        else 
        {
            return KEY_UNPRESSED;
        }
    }
    else 
    {
        return KEY_UNPRESSED;
    }
}

/**
 * @brief 获取当前特定按钮状态函数
 * 
 * @param KeyNum 按钮编号
 * @return Key_Flag 
 */
Key_Flag Key_Get_Current_State(uint8_t KeyNum)
{
    if(KeyNum == 1)
    {
        Key_Flag Key_Temp_State = Key_Current_Flag[KeyNum - 1];
        return Key_Temp_State;
    }
    else 
    {
        return KEY_Flag_ERROR;
    }
}

/**
 * @brief 获取当前特定按钮事件函数
 * 
 * @param KeyNum 按钮编号
 * @return Key_State 
 */
Key_State Key_Get_Event(uint8_t KeyNum)
{
    if(KeyNum == 1)
    {
        Key_State Key_Temp_State = Key_Current_State[KeyNum - 1];
        Key_Current_State[KeyNum - 1] = KEY_FREE;
        return Key_Temp_State;
    }
    else 
    {
        return KEY_ERROR;
    }
}

/**
 * @brief 放入定时器中断 1ms执行一次 按键定时检测函数
 * 
 */
void Key_Tick(void)
{
    static uint16_t Key_Time[KEY_TOTAL_NUM];
    static uint8_t Key_Count;
    static Key_Pressed_State Key_Curr_Preesed_State[KEY_TOTAL_NUM] = {KEY_UNPRESSED};
   
    for(uint8_t i = 0;i < KEY_TOTAL_NUM;i++)
    {
        if(Key_Time[i] > 0)
        {
            Key_Time[i]--;
        }
    }

    Key_Count++;
    
    if(Key_Count >= 20)//20ms进一次 同时作为消抖用
    {
        Key_Count = 0;
        for(uint8_t i = 0;i < KEY_TOTAL_NUM;i++)
        {
            Key_Curr_Preesed_State[i] = Key_Check_PRESSED(i+1);

            if(Key_Current_Flag[i] == KEY_Flag_FREE)//按键空闲标志位时
            {
                if(Key_Curr_Preesed_State[i] == KEY_PRESSED)//按下
                {
                    Key_Time[i] = KEY_TIME_LONG;//设置长按倒计时
                    Key_Current_Flag[i] = KEY_Flag_DOWN;//转入按下标志位
                }
            }
            else if(Key_Current_Flag[i] == KEY_Flag_DOWN)//按键按下标志位时
            {
                if(Key_Curr_Preesed_State[i] == KEY_UNPRESSED)//单击松开 等待确认单击还是双击
                {
                    Key_Time[i] = KEY_TIME_DOUBLE;//设置双击倒计时
                    Key_Current_Flag[i] = KEY_Flag_UP;//转入单击松开标志位

                }
                else if(Key_Time[i] == 0)//长按时间到 确认长按 等待持续长按
                {
                    Key_Time[i] = KEY_TIME_REPEAT;//设置重复倒计时
                    Key_Current_State[i] = KEY_LONG;//确认长按模式
                    Key_Current_Flag[i] = KEY_Flag_LONG;//转入长按标志位
                }
            }
            else if(Key_Current_Flag[i] == KEY_Flag_UP)//按键单击松开标志位时
            {
                if(Key_Curr_Preesed_State[i] == KEY_PRESSED)//确认双击
                {
                    Key_Current_State[i] = KEY_DOUBLE;//确认双击模式
                    Key_Current_Flag[i] = KEY_Flag_DOUBLE;//转入双击标志位
                }
                else if(Key_Time[i] == 0)//双击倒计时到 确认单击
                {
                    Key_Current_State[i] = KEY_SINGLE;//确认单击模式
                    Key_Current_Flag[i] = KEY_Flag_FREE;//返回空闲标志位
                }
            }
            else if(Key_Current_Flag[i] == KEY_Flag_DOUBLE)//按键双击标志位时
            {
                if(Key_Curr_Preesed_State[i] == KEY_UNPRESSED)//确认双击第二击松开
                {
                    Key_Current_Flag[i] = KEY_Flag_FREE;//返回空闲标志位
                }
            }
            else if(Key_Current_Flag[i] == KEY_Flag_LONG)//按键双击标志位时
            {
                if(Key_Curr_Preesed_State[i] == KEY_UNPRESSED)//确认长按松开
                {
                    Key_Current_Flag[i] = KEY_Flag_FREE;//返回空闲标志位
                }
                else if(Key_Time[i] == 0)//重复倒计时到 确认重复
                {
                    Key_Time[i] = KEY_TIME_REPEAT;//设置重复倒计时
                    Key_Current_State[i] = KEY_REPEAT;//确认重复模式
                    Key_Current_Flag[i] = KEY_Flag_REPEAT;//转入重复标志位
                }
            }
            else if(Key_Current_Flag[i] == KEY_Flag_REPEAT)//按键重复标志位时
            {
                if(Key_Curr_Preesed_State[i] == KEY_UNPRESSED)//确认重复松开
                {
                    Key_Current_Flag[i] = KEY_Flag_FREE;//返回空闲标志位
                }
                else if(Key_Time[i] == 0)//重复倒计时到 确认重复
                {
                    Key_Current_State[i] = KEY_REPEAT;//确认重复模式
                    Key_Time[i] = KEY_TIME_REPEAT;//设置重复倒计时
                }
            }

        }
    }
}