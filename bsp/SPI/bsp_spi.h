/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    bsp_spi.h
  * @brief   This file contains all the function prototypes for
  *          the bsp_spi.c file
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal_spi.h"
/*YOUR CODE*/

/**
 * @brief SPI使用GPIO写SS函数（软件模拟SS）
 * 
 * @param GPIO_PIN_State GPIO_PIN_RESET/GPIO_PIN_SET
 */
void SPI_W_SS(GPIO_PinState GPIO_PIN_State);

/**
 * @brief SPI启动函数
 * 
 */
void SPI_Start(void);

/**
 * @brief SPI停止函数
 * 
 */
void SPI_Stop(void);

/**
 * @brief SPI交换一个字节函数
 * 
 * @param hspi hspix
 * @param ByteSend 交换的字节
 * @return uint8_t 返回的字节
 */
uint8_t SPI_SwapByte(SPI_HandleTypeDef *hspi,uint8_t ByteSend);


#ifdef __cplusplus
}
#endif

#endif /* __BSP_SPI_H__ */
