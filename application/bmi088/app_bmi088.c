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
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"
#include <stdint.h>

bmi088_handle_t bmi088_handle;


static uint32_t last_time;


static uint8_t bmi088_readid_acc_flag = 0;
static uint8_t read_accid = 0;
static uint8_t bmi088_readid_gyro_flag = 0;
static uint8_t read_gyroid = 0;

static uint8_t bmi088_gyro_get_raw_data_finished_flag;
static uint8_t bmi088_acc_get_raw_data_finished_flag;
static uint8_t bmi088_acc_get_raw_data_flag;
static uint8_t bmi088_gyro_get_raw_data_flag;

//检查glag
uint8_t checkid_flag = 0;
uint8_t writereg_flag = 0;

typedef enum
{
    init_state_start = 0,
    init_state_accsoftrest,
    init_state_gyrosoftrest,
    init_state_acc_dummyread,
    init_state_readaccidtocheck,
    init_state_readgyroidtocheck,
    init_state_finishidcheck,
    init_state_startconfigreg,
    init_state_finish
} bmi088_init_state_e;

bmi088_init_state_e bmi088_init_state = init_state_start;

typedef struct
{
    int16_t gyro_raw_x;
    int16_t gyro_raw_y;
    int16_t gyro_raw_z;

    int16_t acc_raw_x;
    int16_t acc_raw_y;
    int16_t acc_raw_z;

}bmi088_data_t;

bmi088_data_t bmi088_data;

void bmi088_acc_get_raw_data_finished(int16_t acc_raw_x,int16_t acc_raw_y,int16_t acc_raw_z)
{
    bmi088_data.acc_raw_x = acc_raw_x;
    bmi088_data.acc_raw_y = acc_raw_y;
    bmi088_data.acc_raw_z = acc_raw_z;
    bmi088_gyro_get_raw_data_flag = 1;
    bmi088_acc_get_raw_data_finished_flag = 1;
}

void bmi088_gyro_get_raw_data_finished(int16_t gyro_raw_x,int16_t gyro_raw_y,int16_t gyro_raw_z)
{
    bmi088_data.gyro_raw_x = gyro_raw_x;
    bmi088_data.gyro_raw_y = gyro_raw_y;
    bmi088_data.gyro_raw_z = gyro_raw_z;

    bmi088_gyro_get_raw_data_finished_flag = 1;
}



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
    if(bmi088_init_state == init_state_finish) return;

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
                checkid_flag = 1;
            }
            else 
            {
                JOLED_ShowString(1, 1, "!Check ID Error!");
            }
            JOLED_ShowString(2, 1, "Start write reg");
            bmi088_init_state = init_state_startconfigreg;
        }
    }
    else if(bmi088_init_state ==  init_state_startconfigreg)//配置寄存器
    {
        bmi088_start(&bmi088_handle);
        JOLED_ShowString(2, 1, "  write reg ok ");
        writereg_flag = 1;
        bmi088_init_state = init_state_finish;
        JOLED_ShowString(3, 1, "cnt:");
        if(writereg_flag && checkid_flag)
        {
            JOLED_Clear();
            JOLED_ShowString(4, 1, "BOK");
        }
    }
}



uint32_t last_time1 = 0;

/**
 * @brief BMI088 循环函数
 * 
 */
void app_bmi088_loop(void)
{
    app_bmi088_init_process_loop();
    if(bmi088_init_state == init_state_finish)
    {
        if(bmi088_acc_get_raw_data_flag)
        {
            bmi088_get_acc_raw_data(&bmi088_handle, bmi088_acc_get_raw_data_finished);
        }
        if(bmi088_gyro_get_raw_data_flag)
        {
            bmi088_get_gyro_raw_data(&bmi088_handle, bmi088_gyro_get_raw_data_finished);
        }
        if(HAL_GetTick() - last_time1 >= 30)
        {
            last_time1 += 30;
            if(bmi088_gyro_get_raw_data_finished_flag)
            {
                bmi088_gyro_get_raw_data_finished_flag = 0;
                //JOLED_ShowSignedNum(1, 1, bmi088_data.gyro_raw_x, 4);
                //JOLED_ShowSignedNum(2, 1, bmi088_data.gyro_raw_y, 4);
                //JOLED_ShowSignedNum(3, 1, bmi088_data.gyro_raw_z, 4);
            }
            if(bmi088_acc_get_raw_data_finished_flag)
            {
                bmi088_acc_get_raw_data_finished_flag = 0;
                //JOLED_ShowSignedNum(1, 7, bmi088_data.acc_raw_x, 6);
                //JOLED_ShowSignedNum(2, 7, bmi088_data.acc_raw_y, 6);
                //JOLED_ShowSignedNum(3, 7, bmi088_data.acc_raw_z, 6);
            }
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_11)
    {
        if(bmi088_init_state == init_state_finish)
        {
            bmi088_acc_get_raw_data_flag = 1;
            
            
        }
    }
}