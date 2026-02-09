/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    MPU6050.c
  * @brief   MPU6050使用
  * @details 一般使用是I2C2
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "MPU6050.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_i2c.h"
#include <stdint.h>

#define MPU6050_ADDRESS		0xD0



/**
 * @brief MPU6050读寄存器函数
 * 
 * @param I2C_MPU6050 6050的hi2cx
 * @param RegAddress 需要读的寄存器地址
 * @param Receive_Data 读取出来的数据
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef MPU6050_ReadReg(I2C_HandleTypeDef *I2C_MPU6050,uint8_t RegAddress,uint8_t *Receive_Data)
{
    return HAL_I2C_Mem_Read(I2C_MPU6050, MPU6050_ADDRESS, RegAddress, I2C_MEMADD_SIZE_8BIT, Receive_Data, 1, 100);
}

/**
 * @brief MPU6050写寄存器函数
 * 
 * @param I2C_MPU6050 6050的hi2cx
 * @param RegAddress 需要写的寄存器地址
 * @param WriteData 写的数据
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef MPU6050_WriteReg(I2C_HandleTypeDef *I2C_MPU6050,uint8_t RegAddress,uint8_t WriteData)
{
    return HAL_I2C_Mem_Write(I2C_MPU6050, MPU6050_ADDRESS, RegAddress, I2C_MEMADD_SIZE_8BIT, &WriteData, 1, 100);
}

/**
 * @brief MPU6050获取ID
 * 
 * @param I2C_MPU6050 6050的hi2cx
 * @param Receive_Data 读取出来的数据
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef MPU6050_GetID(I2C_HandleTypeDef *I2C_MPU6050,uint8_t *Receive_Data)
{
    return HAL_I2C_Mem_Read(I2C_MPU6050, MPU6050_ADDRESS, MPU6050_WHO_AM_I, I2C_MEMADD_SIZE_8BIT, Receive_Data, 1, 100);
}

/**
 * @brief MPU6050初始化
 * 
 * @param I2C_MPU6050 6050的hi2cx
 */
void MPU6050_Init(I2C_HandleTypeDef *I2C_MPU6050)
{
    MPU6050_WriteReg(I2C_MPU6050,MPU6050_PWR_MGMT_1,0x01);//电源管理寄存器1 解除睡眠 选择陀螺仪时钟
	MPU6050_WriteReg(I2C_MPU6050,MPU6050_PWR_MGMT_2,0x00);//电源管理寄存器2 6个轴均不待机
	MPU6050_WriteReg(I2C_MPU6050,MPU6050_SMPLRT_DIV,0x09);//采样率分频寄存器 采样分频配置10分频
	MPU6050_WriteReg(I2C_MPU6050,MPU6050_CONFIG,0x06);//配置寄存器 数字低通滤波器给110
	MPU6050_WriteReg(I2C_MPU6050,MPU6050_GYRO_CONFIG,0x10);//陀螺仪配置寄存器  	满量程选择给11（最大
	MPU6050_WriteReg(I2C_MPU6050,MPU6050_ACCEL_CONFIG,0x18);//加速度配置寄存器	满量程选择给11（最大
}

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
void MPU6050_GetData(I2C_HandleTypeDef *I2C_MPU6050,int16_t *AccX,int16_t *AccY,int16_t *AccZ,int16_t *GyroX,int16_t *GyroY,int16_t *GyroZ)
{
	uint8_t DataH,DataL;
	
	//加速度XYZ
	MPU6050_ReadReg(I2C_MPU6050,MPU6050_ACCEL_XOUT_H,&DataH);//读取加速度寄存器高8位
	MPU6050_ReadReg(I2C_MPU6050,MPU6050_ACCEL_XOUT_L,&DataL);//读取加速度寄存器低8位
	*AccX = (DataH << 8) | DataL;//拼接数据 得到加速度计16位数据
	
	MPU6050_ReadReg(I2C_MPU6050,MPU6050_ACCEL_YOUT_H,&DataH);//读取加速度寄存器高8位
	MPU6050_ReadReg(I2C_MPU6050,MPU6050_ACCEL_YOUT_L,&DataL);//读取加速度寄存器低8位
	*AccY = (DataH << 8) | DataL;//得到加速度计16位数据

	MPU6050_ReadReg(I2C_MPU6050,MPU6050_ACCEL_ZOUT_H,&DataH);//读取加速度寄存器高8位
	MPU6050_ReadReg(I2C_MPU6050,MPU6050_ACCEL_ZOUT_L,&DataL);//读取加速度寄存器低8位
	*AccZ = (DataH << 8) | DataL;//得到加速度计16位数据
	
	//陀螺仪XYZ
	MPU6050_ReadReg(I2C_MPU6050,MPU6050_GYRO_XOUT_H,&DataH);//读取陀螺仪寄存器高8位
	MPU6050_ReadReg(I2C_MPU6050,MPU6050_GYRO_XOUT_L,&DataL);//读取陀螺仪寄存器低8位
	*GyroX = (DataH << 8) | DataL;//得到陀螺仪16位数据
	
	MPU6050_ReadReg(I2C_MPU6050,MPU6050_GYRO_YOUT_H,&DataH);//读取陀螺仪寄存器高8位
	MPU6050_ReadReg(I2C_MPU6050,MPU6050_GYRO_YOUT_L,&DataL);//读取陀螺仪寄存器低8位
	*GyroY = (DataH << 8) | DataL;//得到陀螺仪16位数据
	
	MPU6050_ReadReg(I2C_MPU6050,MPU6050_GYRO_ZOUT_H,&DataH);//读取陀螺仪寄存器高8位
	MPU6050_ReadReg(I2C_MPU6050,MPU6050_GYRO_ZOUT_L,&DataL);//读取陀螺仪寄存器低8位
	*GyroZ = (DataH << 8) | DataL;//得到陀螺仪16位数据
}