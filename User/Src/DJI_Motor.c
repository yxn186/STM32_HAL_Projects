/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    DJI_Motor.c
  * @brief   大疆系电机库（目前只支持单种电机多个使用）
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "DJI_Motor.h"
#include "MyCAN.h"
#include <stdint.h>

/**
 * @brief 基准ID 用3508选0x201 用6020选0x205
 * 
 */
#define Motor_ID_Start 0x201   

static DJI_Motor_Data_t *DJI_Motors_Data_Temp = NULL;

/**
 * @brief 大疆电机获取数据函数（回调调用）
 * 
 * @param RxBuffer 
 * @param DJI_Motors_Data 
 */
void DJI_Motor_Get_Data(CAN_Rx_Buffer_t *RxBuffer,DJI_Motor_Data_t *DJI_Motors_Data)
{
    DJI_Motors_Data->RawAngle = (RxBuffer->Data[0] << 8) | RxBuffer->Data[1];
    DJI_Motors_Data->speed_rpm = (int16_t)(((uint16_t)RxBuffer->Data[2] << 8) | RxBuffer->Data[3]);
    DJI_Motors_Data->Torque_Current = (int16_t)(((uint16_t)RxBuffer->Data[4] << 8) | RxBuffer->Data[5]);
    DJI_Motors_Data->Temperature = RxBuffer->Data[6];
}

/**
 * @brief 大疆电机接收回调函数
 * 
 * @param RxBuffer 接收缓冲区
 */
static void DJI_Motor_RxCallBack(CAN_Rx_Buffer_t *RxBuffer)
{
    if (!DJI_Motors_Data_Temp)//防炸
    {
        return;
    }

    uint16_t ID = RxBuffer->Header.StdId;

    if (ID < Motor_ID_Start || ID >= Motor_ID_Start + 8)//防炸
    {
       return;
    }

    DJI_Motor_Get_Data(RxBuffer, &DJI_Motors_Data_Temp[ID - Motor_ID_Start]); 
}

/**
 * @brief 大疆电机CAN初始化函数
 * 
 * @param hcan hcanx
 * @param Object_Para 编号[3:] | FIFOx[2:2] | ID类型[1:1] | 帧类型[0:0]
 * @param ID ID 填写IDx_xxxx
 * @param Mask_ID 屏蔽位
 */
void DJI_Motor_CAN_Init(CAN_HandleTypeDef *hcan, uint8_t Object_Para, uint32_t ID, uint32_t Mask_ID)
{
    CAN_Filter_Mask_Config(hcan,Object_Para,ID,Mask_ID);
    CAN_Init(hcan);
}

/**
 * @brief 大疆电机初始化函数
 * 
 * @param hcan hcanx
 * @param DJI_Motors_Data 电机数据结构体数组[8]（一定要是8！！）
 */
void DJI_Motor_Init(CAN_HandleTypeDef *hcan,DJI_Motor_Data_t DJI_Motors_Data[8])
{
    DJI_Motors_Data_Temp = DJI_Motors_Data;
    CAN_Register_RxCallBack_FIFO0_Function(DJI_Motor_RxCallBack);
    DJI_Motor_CAN_Init(hcan,CAN_FILTER(0) | CAN_FIFO_0 | CAN_STDID | CAN_DATA_TYPE,0x200, 0x7E0);
}

/**
 * @brief 大疆单电机控制函数
 * 
 * @param hcan hcanx
 * @param DJI_Motor_Type 电机型号 DJI_Motor_6020 / DJI_Motor_3508
 * @param DJI_Motor_ID 电机CAN-ID  6020：1~7   3508：1~8
 * @param Out 输出值 6020电压控制：-25000~0~25000   3508电流控制：-16384~0~16384
 */
void DJI_Motor_Control_Single(CAN_HandleTypeDef *hcan,DJI_Motor_Type_Typedef DJI_Motor_Type,uint16_t DJI_Motor_ID,int16_t Out)
{
    static uint8_t TxData8[8] = {0};
    uint16_t TxID = 0;

    //1 判断电机型号 赋值发送ID和限幅Out
    if(DJI_Motor_Type == DJI_Motor_6020)
    {
        if(DJI_Motor_ID > 0 && DJI_Motor_ID <= 4)
        {
            TxID = 0x1FF;
        }
        else if(DJI_Motor_ID > 4 && DJI_Motor_ID <= 7)
        {
            TxID = 0x2FF;
            DJI_Motor_ID = DJI_Motor_ID - 4;
        }

        if(Out >= 25000)
        {
            Out = 25000;
        }
        else if (Out <= -25000)
        {
            Out = -25000;
        }
    }
    else if(DJI_Motor_Type == DJI_Motor_3508)
    {
        if(DJI_Motor_ID > 0 && DJI_Motor_ID <= 4)
        {
            TxID = 0x200;
        }
        else if(DJI_Motor_ID > 4 && DJI_Motor_ID <= 8)
        {
            TxID = 0x1FF;
            DJI_Motor_ID = DJI_Motor_ID - 4;
        }

        if(Out >= 16384)
        {
            Out = 16384;
        }
        else if (Out <= -16384)
        {
            Out = -16384;
        }
    }

    if (TxID == 0)//防炸
    {
        return;
    }

    //2 数据处理 
    uint8_t DJI_Motor_ID_Temp = (uint8_t)(DJI_Motor_ID - 1);
    TxData8[2*DJI_Motor_ID_Temp]   = (uint8_t)((uint16_t)Out >> 8);
    TxData8[2*DJI_Motor_ID_Temp+1] = (uint8_t)((uint16_t)Out);

    // if(DJI_Motor_ID == 1)
    // {
    //     TxData8[0] = Out >> 8;
    //     TxData8[1] = Out;
    // }
    // else if(DJI_Motor_ID == 2)
    // {
    //     TxData8[2] = Out >> 8;
    //     TxData8[3] = Out;
    // }
    // else if(DJI_Motor_ID == 3)
    // {
    //     TxData8[4] = Out >> 8;
    //     TxData8[5] = Out;
    // }
    // else if(DJI_Motor_ID == 4)
    // {
    //     TxData8[6] = Out >> 8;
    //     TxData8[7] = Out;
    // }

    CAN_Send_Data(hcan,TxID,TxData8,8);
}

/**
 * @brief 大疆电机获得角度函数
 * 
 * @param DJI_Motor_ID 电机CAN-ID  6020：1~7   3508：1~8
 * @return float 角度（360度）单位：Deg
 */
float DJI_Motor_Get_Angle(uint8_t DJI_Motor_ID)
{
    return (float)DJI_Motors_Data_Temp[DJI_Motor_ID-1].RawAngle * 360.0 / 8192.0;
}

/**
 * @brief 大疆电机获得角速度函数
 * 
 * @param DJI_Motor_ID 电机CAN-ID  6020：1~7   3508：1~8
 * @return float 角速度 rad/s
 */
float DJI_Motor_Get_AngleSpeed(uint8_t DJI_Motor_ID)
{
    return (float)DJI_Motors_Data_Temp[DJI_Motor_ID-1].speed_rpm / 60.0 * 2.0 * (3.1415926);
}
