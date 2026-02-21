/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    W25Q64.h
  * @brief   This file contains all the function prototypes for
  *          the W25Q64.c file
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __W25Q64_H__
#define __W25Q64_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "bsp_spi.h"
#include <stdint.h>
/*YOUR CODE*/

#define W25Q64_WRITE_ENABLE							0x06
#define W25Q64_WRITE_DISABLE						0x04
#define W25Q64_READ_STATUS_REGISTER_1				0x05
#define W25Q64_READ_STATUS_REGISTER_2				0x35
#define W25Q64_WRITE_STATUS_REGISTER				0x01
#define W25Q64_PAGE_PROGRAM							0x02
#define W25Q64_QUAD_PAGE_PROGRAM					0x32
#define W25Q64_BLOCK_ERASE_64KB						0xD8
#define W25Q64_BLOCK_ERASE_32KB						0x52
#define W25Q64_SECTOR_ERASE_4KB						0x20
#define W25Q64_CHIP_ERASE							0xC7
#define W25Q64_ERASE_SUSPEND						0x75
#define W25Q64_ERASE_RESUME							0x7A
#define W25Q64_POWER_DOWN							0xB9
#define W25Q64_HIGH_PERFORMANCE_MODE				0xA3
#define W25Q64_CONTINUOUS_READ_MODE_RESET			0xFF
#define W25Q64_RELEASE_POWER_DOWN_HPM_DEVICE_ID		0xAB
#define W25Q64_MANUFACTURER_DEVICE_ID				0x90
#define W25Q64_READ_UNIQUE_ID						0x4B
#define W25Q64_JEDEC_ID								0x9F
#define W25Q64_READ_DATA							0x03
#define W25Q64_FAST_READ							0x0B
#define W25Q64_FAST_READ_DUAL_OUTPUT				0x3B
#define W25Q64_FAST_READ_DUAL_IO					0xBB
#define W25Q64_FAST_READ_QUAD_OUTPUT				0x6B
#define W25Q64_FAST_READ_QUAD_IO					0xEB
#define W25Q64_OCTAL_WORD_READ_QUAD_IO				0xE3

#define W25Q64_DUMMY_BYTE							0xFF


/**
 * @brief W25Q64读取ID结束（JEDEC ID）回调函数类型
 * @details user_parameter 是你想从 main 传进来的任意指针（可以传 NULL）
 */
typedef void (*W25Q64_ReadID_FinishedFunction)(uint8_t manufacturer_id,uint16_t device_id,void *user_parameter);

/**
 * @brief W25Q64写使能结束回调函数类型
  * @details user_parameter 是你想从 main 传进来的任意指针（可以传 NULL）
 */
typedef void (*W25Q64_WriteEnable_FinishedFunction)(void *user_parameter);

/**
 * @brief W25Q64等待忙结束回调函数类型
 * @param is_success 1表示成功，0表示失败（参数非法 / 超时等）
 */
typedef void (*W25Q64_WaitBusy_FinishedFunction)(uint8_t is_success, void *user_parameter);

/**
 * @brief W25Q64页编程结束回调函数类型
 * @param is_success 1表示成功，0表示失败（参数非法 / 超时等）
 * @details user_parameter 是你想从 APP 传进来的任意指针（可以传 NULL）
 */
typedef void (*W25Q64_PageProgram_FinishedFunction)(uint8_t is_success,void *user_parameter);

/**
 * @brief W25Q64扇区擦除结束回调函数类型
 * @param is_success 1表示成功，0表示失败（超时等）
 * @details user_parameter 是你想从 APP 传进来的任意指针（可以传 NULL）
 */
typedef void (*W25Q64_SectorErase_FinishedFunction)(uint8_t is_success,void *user_parameter);

/**
 * @brief W25Q64读取数据结束回调函数类型
 * @param is_success 1表示成功，0表示失败（例如长度非法）
 * @param data_array 读取到的数据数组指针（就是你传入的 buffer）
 * @param count 读取字节数
 * @details user_parameter 是你想从 APP 传进来的任意指针（可以传 NULL）
 */
typedef void (*W25Q64_ReadData_FinishedFunction)(uint8_t is_success,uint8_t *data_array,uint32_t count,void *user_parameter);

/**
 * @brief W25Q64运行状态
 * 
 */
typedef enum
{
    W25Q64_NO_OPERATION = 0,

    W25Q64_READING_JEDEC_ID,       /* 读取 JEDEC ID（发送并接收） */

    W25Q64_WRITING_ENABLE,         /* 写使能（仅发送） */

    W25Q64_WAITING_BUSY,           /* 等待忙（反复读状态寄存器1） */

    W25Q64_PAGE_PROGRAM_SENDING,   /* 页编程指令 + 地址 + 数据 已发出，等待发送完成回调 */

    W25Q64_SECTOR_ERASE_SENDING,    /* 扇区擦除指令 + 地址 已发出，等待发送完成回调 */

    W25Q64_READING_DATA,            /* 读取数据中 */

} W25Q64_CurrentOperation;

/**
 * @brief W25Q64句柄
 * 
 */
typedef struct
{
    SPI_HandleTypeDef *hspi;                    //hspix

    GPIO_TypeDef *chip_select_gpiox;            //片选引脚GPIOx
    uint16_t chip_select_pin;                   //片选引脚Pin
    GPIO_PinState chip_select_gpio_pinstate;    //片选引脚状态

    W25Q64_CurrentOperation current_operation;  //目前状态

    /* 这次操作需要的发送缓冲区 / 接收缓冲区
       JEDEC ID 固定 4 字节，所以这里直接给 4 字节数组 */
    uint8_t Tx_buffer[260];
    uint8_t Rx_buffer[260];

    /* 用户回调函数 */
    W25Q64_ReadID_FinishedFunction read_id_finished_function;
    W25Q64_WriteEnable_FinishedFunction write_enable_finished_function;
    W25Q64_WaitBusy_FinishedFunction wait_busy_finished_function;

    W25Q64_PageProgram_FinishedFunction page_program_finished_function;
    W25Q64_SectorErase_FinishedFunction sector_erase_finished_function;

    W25Q64_ReadData_FinishedFunction read_data_finished_function;

    /* 等待忙的内部控制 */
    uint8_t read_busy_flag;
    uint32_t busy_wait_deadline_tick;
    uint32_t next_read_busy_tick;

    /* 页编程：参数记录（异步续接用） */
    uint32_t page_program_address;
    uint16_t page_program_count;
    uint32_t page_program_timeout_ms;

    /* 扇区擦除：参数记录（异步续接用） */
    uint32_t sector_erase_address;
    uint32_t sector_erase_timeout_ms;

    /* 读取数据 */
    uint32_t read_data_address;
    uint32_t read_data_count;
    uint8_t  *read_data_array;

    void *user_parameter;

} W25Q64_Handle_t;

/**
 * @brief W25Q64初始化函数
 * 
 * @param handle W25Q64句柄指针
 * @param hspi hspix 
 * @param chip_select_gpiox 片选GPIOx
 * @param chip_select_pin 片选Pin
 * @param chip_select_gpio_pinstate 片选gpio状态
 */
void W25Q64_Init(W25Q64_Handle_t *handle,SPI_HandleTypeDef *spi_handle,GPIO_TypeDef *chip_select_gpiox,uint16_t chip_select_pin,GPIO_PinState chip_select_gpio_pinstate);

/**
 * @brief W25Q64读取设备、厂商ID函数
 * 
 * @param handle W25Q64句柄
 * @param finished_function W25Q64_ReadID_FinishedFunction函数
 * @param user_parameter 用户参数指针（可选）
 */
void W25Q64_ReadID(W25Q64_Handle_t *handle,W25Q64_ReadID_FinishedFunction finished_function,void *user_parameter);

/**
 * @brief W25Q64写使能函数
 * 
 * @param handle W25Q64句柄
 * @param finished_function W25Q64_WriteEnable_FinishedFunction函数
 * @param user_parameter 用户参数指针（可选）
 */
void W25Q64_WriteEnable(W25Q64_Handle_t *handle,W25Q64_WriteEnable_FinishedFunction finished_function,void *user_parameter);

/**
 * @brief W25Q64等待忙
 * @details 通过反复读取“状态寄存器1”的busy位(bit0)实现
 * @param handle W25Q64句柄
 * @param timeout_ms 超时时间（毫秒）
 * @param finished_function 完成回调函数
 * @param user_parameter 用户参数指针（可选）
 */
void W25Q64_WaitBusy(W25Q64_Handle_t *handle,uint32_t timeout_ms,W25Q64_WaitBusy_FinishedFunction finished_function,void *user_parameter);

/**
 * @brief W25Q64任务循环（用于等待忙的轮询推进）
 * @details 建议在APP层循环里周期调用
 * @param handle W25Q64句柄
 */
void W25Q64_TaskLoop(W25Q64_Handle_t *handle);

/**
 * @brief W25Q64页编程
 *
 * @details
 * 1) 发送写使能 0x06（异步）
 * 2) 写使能完成后，发送页编程指令0x02 + 3字节地址 + 数据（异步）
 * 3) 发送完成后，进入等待忙（异步轮询状态寄存器1）
 * 4) 等待忙结束后，通过 finished_function 通知成功或失败
 *
 * @param handle W25Q64句柄
 * @param address 写入起始地址（0x000000~0x7FFFFF）
 * @param data_array 待写入数据数组指针
 * @param count 写入字节数（1~256）
 * @param timeout_ms 等待忙超时时间（毫秒）
 * @param finished_function 完成回调函数
 * @param user_parameter 用户参数指针（可选）
 */
void W25Q64_PageProgram(W25Q64_Handle_t *handle,uint32_t address, uint8_t *data_array,uint16_t count,uint32_t timeout_ms,W25Q64_PageProgram_FinishedFunction finished_function,void *user_parameter);

/**
 * @brief W25Q64扇区擦除
 *
 * @details
 * 1) 发送写使能 0x06
 * 2) 写使能完成后，发送扇区擦除指令0x20 + 3字节地址（异步）
 * 3) 发送完成后，进入等待忙（异步轮询状态寄存器1）
 * 4) 等待忙结束后，通过 finished_function 通知成功或失败
 *
 * @param handle W25Q64句柄
 * @param address 扇区内任意地址（会擦除该地址所在4KB扇区）
 * @param timeout_ms 等待忙超时时间（毫秒）
 * @param finished_function 完成回调函数
 * @param user_parameter 用户参数指针（可选）
 */
void W25Q64_SectorErase(W25Q64_Handle_t *handle,uint32_t address,uint32_t timeout_ms,W25Q64_SectorErase_FinishedFunction finished_function,void *user_parameter);

/**
 * @brief W25Q64读取数据
 * @details 发送 0x03 + 3字节地址，然后发送占位字节产生时钟，读取 count 字节数据
 * @param handle W25Q64句柄
 * @param address 读取起始地址
 * @param data_array 接收数据数组指针（必须在回调前保持有效）
 * @param count 读取字节数（建议 <= 256，超过也能读但受你缓冲区260限制）
 * @param finished_function 完成回调函数
 * @param user_parameter 用户参数指针（可选）
 */
void W25Q64_ReadData(W25Q64_Handle_t *handle,
                     uint32_t address,
                     uint8_t *data_array,
                     uint32_t count,
                     W25Q64_ReadData_FinishedFunction finished_function,
                     void *user_parameter);

/**
 * @brief W25Q64TxRx回调函数
 * 
 * @param Tx_Buffer 发送缓冲区
 * @param Rx_Buffer 接收缓冲区
 * @param Tx_Length 发送长度
 * @param Rx_Length 接受长度
 */
void W25Q64_SPI_TxRxCallback(uint8_t *Tx_Buffer,uint8_t *Rx_Buffer,uint16_t Tx_Length,uint16_t Rx_Length);

#ifdef __cplusplus
}
#endif

#endif /* __W25Q64_H__ */
