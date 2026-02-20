/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    bsp_spi.c
  * @brief   SPI库
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "bsp_spi.h"
#include "stm32f1xx_hal_gpio.h"
#include <stdint.h>

/**
 * @brief SPI使用GPIO写SS函数（软件模拟SS）
 * 
 * @param GPIO_PIN_State GPIO_PIN_RESET/GPIO_PIN_SET
 */
void SPI_W_SS(GPIO_PinState GPIO_PIN_State)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_State);
}

/**
 * @brief SPI启动函数
 * 
 */
void SPI_Start(void)
{
	SPI_W_SS(GPIO_PIN_RESET);
}

/**
 * @brief SPI停止函数
 * 
 */
void SPI_Stop(void)
{
	SPI_W_SS(GPIO_PIN_SET);
}

/**
 * @brief SPI交换一个字节函数
 * 
 * @param hspi hspix
 * @param ByteSend 交换的字节
 * @return uint8_t 返回的字节
 */
uint8_t SPI_SwapByte(SPI_HandleTypeDef *hspi,uint8_t ByteSend)
{
    uint8_t TxData,RxData;
    TxData = ByteSend;
    HAL_SPI_TransmitReceive(hspi, &TxData, &RxData, 1, HAL_MAX_DELAY);
    return RxData;
}
