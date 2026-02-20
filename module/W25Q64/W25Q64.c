/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    W25Q64.c
  * @brief   W25Q64库
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "W25Q64.h"

/**
 * @brief W25Q64读取ID号
 * 
 * @param MID 工厂ID，使用输出参数的形式返回
 * @param DID 设备ID，使用输出参数的形式返回
 */
void W25Q64_ReadID(SPI_HandleTypeDef *hspi,uint8_t *MID, uint16_t *DID)
{
	SPI_Start();								//SPI起始
    SPI_SwapByte(hspi,W25Q64_JEDEC_ID);			//交换发送读取ID的指令
	*MID = SPI_SwapByte(hspi,W25Q64_DUMMY_BYTE);	//交换接收MID，通过输出参数返回
	*DID = SPI_SwapByte(hspi,W25Q64_DUMMY_BYTE);	//交换接收DID高8位
	*DID <<= 8;									//高8位移到高位
	*DID |= SPI_SwapByte(hspi,W25Q64_DUMMY_BYTE);	//或上交换接收DID的低8位，通过输出参数返回
	SPI_Stop();		  						//SPI终止
}

/**
 * @brief W25Q64写使能
 * 
 */
void W25Q64_WriteEnable(SPI_HandleTypeDef *hspi)
{
	SPI_Start();								//SPI起始
	SPI_SwapByte(hspi,W25Q64_WRITE_ENABLE);		//交换发送写使能的指令
	SPI_Stop();								//SPI终止
}

/**
 * @brief W25Q64等待忙
 * 
 */
void W25Q64_WaitBusy(SPI_HandleTypeDef *hspi)
{
	uint32_t Timeout;
	SPI_Start();								//SPI起始
	SPI_SwapByte(hspi,W25Q64_READ_STATUS_REGISTER_1);				//交换发送读状态寄存器1的指令
	Timeout = 100000;							//给定超时计数时间
	while ((SPI_SwapByte(hspi,W25Q64_DUMMY_BYTE) & 0x01) == 0x01)	//循环等待忙标志位
	{
		Timeout --;								//等待时，计数值自减
		if (Timeout == 0)						//自减到0后，等待超时
		{
			/*超时的错误处理代码，可以添加到此处*/
			break;								//跳出等待，不等了
		}
	}
	SPI_Stop();								//SPI终止
}

/**
 * @brief W25Q64页编程
 * 
 * @param Address 页编程的起始地址，范围：0x000000~0x7FFFFF
 * @param DataArray 用于写入数据的数组
 * @param Count 要写入数据的数量，范围：0~256
 * @details 写入的地址范围不能跨页
 */
void W25Q64_PageProgram(SPI_HandleTypeDef *hspi,uint32_t Address, uint8_t *DataArray, uint16_t Count)
{
	uint16_t i;
	
	W25Q64_WriteEnable(hspi);						//写使能
	
	SPI_Start();								//SPI起始
	SPI_SwapByte(hspi,W25Q64_PAGE_PROGRAM);		//交换发送页编程的指令
	SPI_SwapByte(hspi,Address >> 16);				//交换发送地址23~16位
	SPI_SwapByte(hspi,Address >> 8);				//交换发送地址15~8位
	SPI_SwapByte(hspi,Address);					//交换发送地址7~0位
	for (i = 0; i < Count; i ++)				//循环Count次
	{
		SPI_SwapByte(hspi,DataArray[i]);			//依次在起始地址后写入数据 低位先写入  
	}
	SPI_Stop();								//SPI终止
	
	W25Q64_WaitBusy(hspi);							//等待忙
}

/**
 * @brief W25Q64扇区擦除（4KB）
 * 
 * @param Address 指定扇区的地址，范围：0x000000~0x7FFFFF
 */
void W25Q64_SectorErase(SPI_HandleTypeDef *hspi,uint32_t Address)
{
	W25Q64_WriteEnable(hspi);						//写使能

	SPI_Start();								//SPI起始
	SPI_SwapByte(hspi,W25Q64_SECTOR_ERASE_4KB);	//交换发送扇区擦除的指令
	SPI_SwapByte(hspi,Address >> 16);				//交换发送地址23~16位
	SPI_SwapByte(hspi,Address >> 8);				//交换发送地址15~8位
	SPI_SwapByte(hspi,Address);					//交换发送地址7~0位
	SPI_Stop();								//SPI终止
	
	W25Q64_WaitBusy(hspi);							//等待忙
}

/**
 * @brief W25Q64读取数据
 * 
 * @param Address 读取数据的起始地址，范围：0x000000~0x7FFFFF
 * @param DataArray 用于接收读取数据的数组，通过输出参数返回
 * @param Count 要读取数据的数量，范围：0~0x800000
 */
void W25Q64_ReadData(SPI_HandleTypeDef *hspi,uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
	uint32_t i;
	SPI_Start();								//SPI起始
	SPI_SwapByte(hspi,W25Q64_READ_DATA);			//交换发送读取数据的指令
	SPI_SwapByte(hspi,Address >> 16);				//交换发送地址23~16位
	SPI_SwapByte(hspi,Address >> 8);				//交换发送地址15~8位
	SPI_SwapByte(hspi,Address);					//交换发送地址7~0位
	for (i = 0; i < Count; i ++)				//循环Count次
	{
		DataArray[i] = SPI_SwapByte(hspi,W25Q64_DUMMY_BYTE);	//依次在起始地址后读取数据
	}
	SPI_Stop();								//SPI终止
}