/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    MPU6050.h
  * @brief   This file contains all the function prototypes for
  *          the MPU6050.c file
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MPU6050_H__
#define __MPU6050_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/*YOUR CODE*/

/* MPU6050寄存器地址 */
#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			0x1A
#define	MPU6050_GYRO_CONFIG		0x1B
#define	MPU6050_ACCEL_CONFIG	0x1C

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B 	//电源管理寄存器1
#define	MPU6050_PWR_MGMT_2		0x6C	//电源管理寄存器2
#define	MPU6050_WHO_AM_I		0x75
/* MPU6050寄存器地址 */

/**
 * @brief MPU6050读寄存器函数
 * 
 * @param I2C_MPU6050 6050的hi2cx
 * @param RegAddress 需要读的寄存器地址
 * @param Receive_Data 读取出来的数据
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef MPU6050_ReadReg(I2C_HandleTypeDef *I2C_MPU6050,uint8_t RegAddress,uint8_t *Receive_Data);

/**
 * @brief MPU6050写寄存器函数
 * 
 * @param I2C_MPU6050 6050的hi2cx
 * @param RegAddress 需要写的寄存器地址
 * @param WriteData 写的数据
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef MPU6050_WriteReg(I2C_HandleTypeDef *I2C_MPU6050,uint8_t RegAddress,uint8_t WriteData);

/**
 * @brief MPU6050获取ID
 * 
 * @param I2C_MPU6050 6050的hi2cx
 * @param Receive_Data 读取出来的数据
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef MPU6050_GetID(I2C_HandleTypeDef *I2C_MPU6050,uint8_t *Receive_Data);

/**
 * @brief MPU6050初始化
 * 
 * @param I2C_MPU6050 6050的hi2cx
 */
void MPU6050_Init(I2C_HandleTypeDef *I2C_MPU6050);

/**
 * @brief 获得MPU6050数据
 * 
 * @param I2C_MPU6050 6050的hi2cx
 * @param AccX 加速度X
 * @param AccY 加速度Y
 * @param AccZ 加速度Z
 * @param GyroX 陀螺仪X
 * @param GyroY 陀螺仪Y
 * @param GyroZ 陀螺仪Z
 */
void MPU6050_GetData(I2C_HandleTypeDef *I2C_MPU6050,int16_t *AccX,int16_t *AccY,int16_t *AccZ,int16_t *GyroX,int16_t *GyroY,int16_t *GyroZ);


#ifdef __cplusplus
}
#endif

#endif /* __MPU6050_H__ */
