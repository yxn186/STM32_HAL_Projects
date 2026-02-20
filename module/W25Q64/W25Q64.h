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
 * @brief W25Q64读取ID号
 * 
 * @param MID 工厂ID，使用输出参数的形式返回
 * @param DID 设备ID，使用输出参数的形式返回
 */
void W25Q64_ReadID(SPI_HandleTypeDef *hspi,uint8_t *MID, uint16_t *DID);

/**
 * @brief W25Q64写使能
 * 
 */
void W25Q64_WriteEnable(SPI_HandleTypeDef *hspi);

/**
 * @brief W25Q64等待忙
 * 
 */
void W25Q64_WaitBusy(SPI_HandleTypeDef *hspi);

/**
 * @brief W25Q64页编程
 * 
 * @param Address 页编程的起始地址，范围：0x000000~0x7FFFFF
 * @param DataArray 用于写入数据的数组
 * @param Count 要写入数据的数量，范围：0~256
 * @details 写入的地址范围不能跨页
 */
void W25Q64_PageProgram(SPI_HandleTypeDef *hspi,uint32_t Address, uint8_t *DataArray, uint16_t Count);

/**
 * @brief W25Q64扇区擦除（4KB）
 * 
 * @param Address 指定扇区的地址，范围：0x000000~0x7FFFFF
 */
void W25Q64_SectorErase(SPI_HandleTypeDef *hspi,uint32_t Address);

/**
 * @brief W25Q64读取数据
 * 
 * @param Address 读取数据的起始地址，范围：0x000000~0x7FFFFF
 * @param DataArray 用于接收读取数据的数组，通过输出参数返回
 * @param Count 要读取数据的数量，范围：0~0x800000
 */
void W25Q64_ReadData(SPI_HandleTypeDef *hspi,uint32_t Address, uint8_t *DataArray, uint32_t Count);

#ifdef __cplusplus
}
#endif

#endif /* __W25Q64_H__ */
