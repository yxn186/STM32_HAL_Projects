/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_bmi088.c
  * @brief   bmi088 app层
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "app_bmi088.h"
#include "bmi088.h"
#include "bmi088reg.h"
#include "joled.h"
#include "spi.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include <stdint.h>

bmi088_handle_t bmi088_handle;


static uint32_t last_time;

static uint8_t bmi088_readid_acc_flag = 0;
static uint8_t read_accid = 0;
static uint8_t bmi088_readid_gyro_flag = 0;
static uint8_t read_gyroid = 0;

typedef enum
{
    init_state_start = 0,
    init_state_accsoftrest,
    init_state_gyrosoftrest,
    init_state_acc_dummyread,
    init_state_readaccidtocheck,
    init_state_readgyroidtocheck,
    init_state_finishidcheck,
    init_state_finish
} bmi088_init_state_e;

bmi088_init_state_e bmi088_init_state = init_state_start;

static void bmi088_readid_acc_finished(uint8_t accid)
{
    read_accid = accid;
    bmi088_readid_acc_flag = 1;
}

static void bmi088_readid_gyro_finished(uint8_t gyroid)
{
    read_gyroid = gyroid;
    bmi088_readid_gyro_flag = 1;
}

/**
 * @brief BMI088初始化
 * 
 */
void app_bmi088_init(void)
{
    bmi088_init(&bmi088_handle, &hspi1, GPIOA, GPIO_PIN_4, GPIO_PIN_RESET, GPIOA, GPIO_PIN_3, GPIO_PIN_RESET, GPIOB, GPIO_PIN_11, GPIOB, GPIO_PIN_10);
    bmi088_init_state = init_state_accsoftrest;
}

void app_bmi088_task1(void)
{
    bmi088_readid_acc(&bmi088_handle, bmi088_readid_acc_finished);
}

void app_bmi088_task2(void)
{
    bmi088_readid_gyro(&bmi088_handle, bmi088_readid_gyro_finished);
}

void app_bmi088_init_process_loop(void)
{
    if(bmi088_init_state == init_state_accsoftrest)//acc软复位
    {
        bmi088_acc_softreset(&bmi088_handle,NULL);
        last_time = HAL_GetTick();
        bmi088_init_state = init_state_gyrosoftrest;
    }
    else if(bmi088_init_state == init_state_gyrosoftrest)//gyro软复位
    {
        if(HAL_GetTick() - last_time >= 20)//等待1ms以上软复位完成
        {
            bmi088_gyro_softreset(&bmi088_handle,NULL);
            last_time = HAL_GetTick();
            bmi088_init_state = init_state_acc_dummyread;
        }
    }
    else if(bmi088_init_state == init_state_acc_dummyread)//acc舍去一次无效读写
    {
        if(HAL_GetTick() - last_time >= 40)//等待30ms以上软复位完成
        {
            bmi088_readid_acc(&bmi088_handle, bmi088_readid_acc_finished);
            last_time = HAL_GetTick();
            bmi088_init_state = init_state_readaccidtocheck;
        }
    }
    else if(bmi088_init_state == init_state_readaccidtocheck)//读accid
    {
        if(bmi088_readid_acc_flag)
        {
            bmi088_readid_acc(&bmi088_handle, bmi088_readid_acc_finished);
            bmi088_init_state = init_state_readgyroidtocheck;
            bmi088_readid_acc_flag = 0;
        }
    }
    else if(bmi088_init_state == init_state_readgyroidtocheck)//读gyroid
    {
        if(bmi088_readid_acc_flag)
        {
            JOLED_ShowHexNum(1, 1, read_accid, 2);
            bmi088_readid_gyro(&bmi088_handle, bmi088_readid_gyro_finished);
            bmi088_init_state = init_state_finishidcheck;
            bmi088_readid_acc_flag = 0;
        }
    }
    else if(bmi088_init_state == init_state_finishidcheck)//检查读取的id
    {
        if(bmi088_readid_gyro_flag)
        {
            JOLED_ShowHexNum(1, 4, read_gyroid, 2);
            bmi088_readid_gyro_flag = 0;
            if(read_accid == BMI088_ACC_CHIP_ID_VALUE && read_gyroid == BMI088_GYRO_CHIP_ID_VALUE)
            {
                JOLED_ShowString(1, 1, "  Check ID OK!   ");
            }
            else 
            {
                JOLED_ShowString(1, 1, "!Check ID Error!");
            }
            bmi088_init_state = init_state_finish;
        }
    }

}

/**
 * @brief BMI088 循环函数
 * 
 */
void app_bmi088_loop(void)
{
    app_bmi088_init_process_loop();
}
