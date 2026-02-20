/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    bsp_spi.cpp
  * @brief   SPI库
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "bsp_spi.h"
#include "stm32f1xx_hal_gpio.h"
#include <cstddef>
#include <stdint.h>

/* Private variables ---------------------------------------------------------*/

Struct_SPI_Manage_Object SPI1_Manage_Object = {nullptr};
Struct_SPI_Manage_Object SPI2_Manage_Object = {nullptr};
Struct_SPI_Manage_Object SPI3_Manage_Object = {nullptr};
Struct_SPI_Manage_Object SPI4_Manage_Object = {nullptr};
Struct_SPI_Manage_Object SPI5_Manage_Object = {nullptr};
Struct_SPI_Manage_Object SPI6_Manage_Object = {nullptr};

/* Private function declarations ---------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/**
 * @brief 初始化SPI
 *
 * @param hspi SPI编号
 * @param Callback_Function 处理回调函数
 */
void SPI_Init(SPI_HandleTypeDef *hspi, SPI_Callback Callback_Function)
{
    if (hspi->Instance == SPI1)
    {
        SPI1_Manage_Object.SPI_Handler = hspi;
        SPI1_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (hspi->Instance == SPI2)
    {
        SPI2_Manage_Object.SPI_Handler = hspi;
        SPI2_Manage_Object.Callback_Function = Callback_Function;
    }
}

/**
 * @brief 发送数据
 *
 * @param hspi SPI编号
 * @param GPIOx 片选GPIO引脚编组
 * @param GPIO_Pin 片选GPIO引脚号
 * @param Activate_Level 片选GPIO引脚电平
 * @param Tx_Length 长度
 * @return uint8_t 执行状态
 */
uint8_t SPI_Transmit_Data(SPI_HandleTypeDef *hspi, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState Activate_Level, uint8_t *Tx_Buffer, uint16_t Tx_Length)
{
    if (hspi->Instance == SPI1)
    {
        SPI1_Manage_Object.Activate_GPIOx = GPIOx;
        SPI1_Manage_Object.Activate_GPIO_Pin = GPIO_Pin;
        SPI1_Manage_Object.Activate_Level = Activate_Level;
        SPI1_Manage_Object.Tx_Buffer_Length = Tx_Length;
        SPI1_Manage_Object.Rx_Buffer_Length = 0;
        SPI1_Manage_Object.Tx_Buffer = Tx_Buffer;
        if (SPI1_Manage_Object.Activate_GPIOx != nullptr)
        {
            HAL_GPIO_WritePin(SPI1_Manage_Object.Activate_GPIOx, SPI1_Manage_Object.Activate_GPIO_Pin, Activate_Level);
        }

        return (HAL_SPI_Transmit_DMA(hspi, SPI1_Manage_Object.Tx_Buffer, Tx_Length));
    }
    else if (hspi->Instance == SPI2)
    {
        SPI2_Manage_Object.Activate_GPIOx = GPIOx;
        SPI2_Manage_Object.Activate_GPIO_Pin = GPIO_Pin;
        SPI2_Manage_Object.Activate_Level = Activate_Level;
        SPI2_Manage_Object.Tx_Buffer_Length = Tx_Length;
        SPI2_Manage_Object.Rx_Buffer_Length = 0;
        SPI2_Manage_Object.Tx_Buffer = Tx_Buffer;
        if (SPI2_Manage_Object.Activate_GPIOx != nullptr)
        {
            HAL_GPIO_WritePin(SPI2_Manage_Object.Activate_GPIOx, SPI2_Manage_Object.Activate_GPIO_Pin, Activate_Level);
        }

        return (HAL_SPI_Transmit_DMA(hspi, SPI2_Manage_Object.Tx_Buffer, Tx_Length));
    }

    return (HAL_ERROR);
}

/**
 * @brief 交互数据帧
 * 
 * @param hspi SPI编号
 * @param GPIOx 片选GPIO引脚编组
 * @param GPIO_Pin 片选GPIO引脚号
 * @param Activate_Level 片选GPIO引脚电平
 * @param Tx_Length 发送数据长度
 * @param Rx_Length 接收数据长度
 * @return uint8_t 执行状态
 */
uint8_t SPI_Transmit_Receive_Data(SPI_HandleTypeDef *hspi, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState Activate_Level,uint8_t *Tx_Buffer,uint8_t *Rx_Buffer, uint16_t Tx_Length, uint16_t Rx_Length)
{
    if (hspi->Instance == SPI1)
    {
        SPI1_Manage_Object.Activate_GPIOx = GPIOx;
        SPI1_Manage_Object.Activate_GPIO_Pin = GPIO_Pin;
        SPI1_Manage_Object.Activate_Level = Activate_Level;
        SPI1_Manage_Object.Tx_Buffer_Length = Tx_Length;
        SPI1_Manage_Object.Rx_Buffer_Length = Rx_Length;
        SPI1_Manage_Object.Tx_Buffer = Tx_Buffer;
        SPI1_Manage_Object.Rx_Buffer = Rx_Buffer;

        if (SPI1_Manage_Object.Activate_GPIOx != nullptr)
        {
            HAL_GPIO_WritePin(SPI1_Manage_Object.Activate_GPIOx, SPI1_Manage_Object.Activate_GPIO_Pin, Activate_Level);
        }

        return (HAL_SPI_TransmitReceive_DMA(hspi, SPI1_Manage_Object.Tx_Buffer, SPI1_Manage_Object.Rx_Buffer, Tx_Length + Rx_Length));
    }
    else if (hspi->Instance == SPI2)
    {
        SPI2_Manage_Object.Activate_GPIOx = GPIOx;
        SPI2_Manage_Object.Activate_GPIO_Pin = GPIO_Pin;
        SPI2_Manage_Object.Activate_Level = Activate_Level;
        SPI2_Manage_Object.Tx_Buffer_Length = Tx_Length;
        SPI2_Manage_Object.Rx_Buffer_Length = Rx_Length;
        SPI2_Manage_Object.Tx_Buffer = Tx_Buffer;
        SPI2_Manage_Object.Rx_Buffer = Rx_Buffer;

        if (SPI2_Manage_Object.Activate_GPIOx != nullptr)
        {
            HAL_GPIO_WritePin(SPI2_Manage_Object.Activate_GPIOx, SPI2_Manage_Object.Activate_GPIO_Pin, Activate_Level);
        }

        return (HAL_SPI_TransmitReceive_DMA(hspi, SPI2_Manage_Object.Tx_Buffer, SPI2_Manage_Object.Rx_Buffer, Tx_Length + Rx_Length));
    }

    return (HAL_ERROR);
}

/**
 * @brief HAL库SPI仅发送回调函数
 *
 * @param hspi SPI编号
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1)
    {
        if (SPI1_Manage_Object.Activate_GPIOx != nullptr)
        {
            if (SPI1_Manage_Object.Activate_Level == GPIO_PIN_SET)
            {
                HAL_GPIO_WritePin(SPI1_Manage_Object.Activate_GPIOx, SPI1_Manage_Object.Activate_GPIO_Pin, GPIO_PIN_RESET);
            }
            else
            {
                HAL_GPIO_WritePin(SPI1_Manage_Object.Activate_GPIOx, SPI1_Manage_Object.Activate_GPIO_Pin, GPIO_PIN_SET);
            }
        }

        if (SPI1_Manage_Object.Callback_Function != nullptr)
        {
            SPI1_Manage_Object.Callback_Function(SPI1_Manage_Object.Tx_Buffer, NULL, SPI1_Manage_Object.Tx_Buffer_Length, 0);
        }

        SPI1_Manage_Object.Activate_GPIOx = nullptr;
    }
    else if (hspi->Instance == SPI2)
    {
        if (SPI2_Manage_Object.Activate_GPIOx != nullptr)
        {
            if (SPI2_Manage_Object.Activate_Level == GPIO_PIN_SET)
            {
                HAL_GPIO_WritePin(SPI2_Manage_Object.Activate_GPIOx, SPI2_Manage_Object.Activate_GPIO_Pin, GPIO_PIN_RESET);
            }
            else
            {
                HAL_GPIO_WritePin(SPI2_Manage_Object.Activate_GPIOx, SPI2_Manage_Object.Activate_GPIO_Pin, GPIO_PIN_SET);
            }
        }
        
        if (SPI2_Manage_Object.Callback_Function != nullptr)
        {
            SPI2_Manage_Object.Callback_Function(SPI2_Manage_Object.Tx_Buffer, NULL, SPI2_Manage_Object.Tx_Buffer_Length, 0);
        }

        SPI2_Manage_Object.Activate_GPIOx = nullptr;
    }
}

/**
 * @brief HAL库SPI全双工DMA中断
 * 
 * @param hspi SPI编号
 */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    // 选择回调函数
    if (hspi->Instance == SPI1)
    {
        if (SPI1_Manage_Object.Activate_GPIOx != nullptr)
        {
            if (SPI1_Manage_Object.Activate_Level == GPIO_PIN_SET)
            {
                HAL_GPIO_WritePin(SPI1_Manage_Object.Activate_GPIOx, SPI1_Manage_Object.Activate_GPIO_Pin, GPIO_PIN_RESET);
            }
            else
            {
                HAL_GPIO_WritePin(SPI1_Manage_Object.Activate_GPIOx, SPI1_Manage_Object.Activate_GPIO_Pin, GPIO_PIN_SET);
            }
        }

        //SPI1_Manage_Object.Rx_Timestamp = SYS_Timestamp.Get_Current_Timestamp();

        if (SPI1_Manage_Object.Callback_Function != nullptr)
        {
            SPI1_Manage_Object.Callback_Function(SPI1_Manage_Object.Tx_Buffer, SPI1_Manage_Object.Rx_Buffer, SPI1_Manage_Object.Tx_Buffer_Length, SPI1_Manage_Object.Rx_Buffer_Length);
        }

        SPI1_Manage_Object.Activate_GPIOx = nullptr;
    }
    else if (hspi->Instance == SPI2)
    {
        if (SPI2_Manage_Object.Activate_GPIOx != nullptr)
        {
            if (SPI2_Manage_Object.Activate_Level == GPIO_PIN_SET)
            {
                HAL_GPIO_WritePin(SPI2_Manage_Object.Activate_GPIOx, SPI2_Manage_Object.Activate_GPIO_Pin, GPIO_PIN_RESET);
            }
            else
            {
                HAL_GPIO_WritePin(SPI2_Manage_Object.Activate_GPIOx, SPI2_Manage_Object.Activate_GPIO_Pin, GPIO_PIN_SET);
            }
        }

        //SPI2_Manage_Object.Rx_Timestamp = SYS_Timestamp.Get_Current_Timestamp();

        if (SPI2_Manage_Object.Callback_Function != nullptr)
        {
            SPI2_Manage_Object.Callback_Function(SPI2_Manage_Object.Tx_Buffer, SPI2_Manage_Object.Rx_Buffer, SPI2_Manage_Object.Tx_Buffer_Length, SPI2_Manage_Object.Rx_Buffer_Length);
        }

        SPI2_Manage_Object.Activate_GPIOx = nullptr;
    }
}