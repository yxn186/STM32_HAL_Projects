/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_W25Q64.c
  * @brief   W25Q64 App层
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "app_W25Q64.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal_gpio.h"
#include "w25q64.h"
#include "joled.h"
#include "spi.h"
/* APP 层保存“业务需要的结果” */
W25Q64_Handle_t W25Q64_Handle;

static volatile uint8_t g_flash_id_ready = 0;
static uint8_t  g_manufacturer_id = 0;
static uint16_t g_device_id = 0;

/* APP层：页编程完成标志 */
static volatile uint8_t g_page_program_done = 0;
static volatile uint8_t g_page_program_success = 0;
static uint8_t g_tx_buffer[] = {0x01,0x09,0x03,0x04};;

/* APP层：扇区擦除完成标志 */
static volatile uint8_t g_sector_erase_done = 0;
static volatile uint8_t g_sector_erase_success = 0;

/* APP层：读取数据标志+存放缓冲区 */
static volatile uint8_t g_read_done = 0;
static volatile uint8_t g_read_success = 0;
static uint8_t g_read_buffer[16];

static void ReadID_Finished(uint8_t manufacturer_id,uint16_t device_id,void *user_parameter)
{
    (void)user_parameter;

    g_manufacturer_id = manufacturer_id;
    g_device_id = device_id;
    g_flash_id_ready = 1;   /* 通知主循环：结果已经准备好 */
}

static void PageProgram_Finished(uint8_t is_success, void *user_parameter)
{
    (void)user_parameter;
    g_page_program_success = is_success;
    g_page_program_done = 1;
}

static void SectorErase_Finished(uint8_t is_success, void *user_parameter)
{
    (void)user_parameter;
    g_sector_erase_success = is_success;
    g_sector_erase_done = 1;
}

static void ReadData_Finished(uint8_t is_success,uint8_t *data_array,uint32_t count,void *user_parameter)
{
    (void)user_parameter;
    (void)data_array;
    (void)count;

    g_read_success = is_success;
    g_read_done = 1;
}

void App_W25Q64_Init(void)
{
    /* 这里做你需要的初始化，例如：
       - W25Q64 驱动初始化（绑定 SPI 句柄、片选引脚）
       - 发起一次异步读 ID，并把“完成回调函数”传进去
    */
    W25Q64_Init(&W25Q64_Handle,&hspi1,GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    W25Q64_ReadID(&W25Q64_Handle,ReadID_Finished, NULL);
}

void App_W25Q64_Task3(void)
{
    W25Q64_ReadData(&W25Q64_Handle, 0x000000, g_read_buffer, 16, ReadData_Finished, NULL);
}

void App_W25Q64_Task1(void)
{
     W25Q64_SectorErase(&W25Q64_Handle, 0x000000, 100, SectorErase_Finished, NULL);
}
   

void App_W25Q64_Task2(void)
{
    W25Q64_PageProgram(&W25Q64_Handle, 0x000000, g_tx_buffer, 4, 1000, PageProgram_Finished, NULL);
}

void App_W25Q64_TaskLoop(void)
{
    W25Q64_TaskLoop(&W25Q64_Handle);

    if (g_flash_id_ready)
    {
        g_flash_id_ready = 0;

        /* 这里再做 OLED 显示、串口打印等“业务动作” */
        JOLED_Clear();
        JOLED_ShowString(1,1,"MID:   DID:");
        JOLED_ShowHexNum(1,5,g_manufacturer_id,2);
        JOLED_ShowHexNum(1,12,g_device_id,4);
        JOLED_ShowString(2,1,"W:");
	    JOLED_ShowString(3,1,"R:");
        
        App_W25Q64_Task1();
    }

    if (g_sector_erase_done)
    {
        g_sector_erase_done = 0;
        if (g_sector_erase_success)
        {
            App_W25Q64_Task2();
        }
        else
        {
            /* 擦除失败：提示/重试 */
        }
    }

    if (g_page_program_done)
    {
        g_page_program_done = 0;
        if (g_page_program_success)
        {
            JOLED_ShowHexNum(2,3,g_tx_buffer[0],2);
            JOLED_ShowHexNum(2,6,g_tx_buffer[1],2);
            JOLED_ShowHexNum(2,9,g_tx_buffer[2],2);
            JOLED_ShowHexNum(2,12,g_tx_buffer[3],2);
            App_W25Q64_Task3();
        }
        else
        {
            /* 写入失败：提示/重试 */
        }
    }

    if(g_read_done)
    {
        g_read_done = 0;
        if (g_read_success) 
        {
            JOLED_ShowHexNum(3,3,g_read_buffer[0],2);
            JOLED_ShowHexNum(3,6,g_read_buffer[1],2);
            JOLED_ShowHexNum(3,9,g_read_buffer[2],2);
            JOLED_ShowHexNum(3,12,g_read_buffer[3],2);
        }
    }
}

