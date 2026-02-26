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
#include "bsp_spi.h"
#include <string.h>

static W25Q64_Handle_t *W25Q64_Handle_Global = NULL;

/* 内部续接函数声明 */
static void W25Q64_PageProgram_WriteEnableFinished(void *user_parameter);
static void W25Q64_SectorErase_WriteEnableFinished(void *user_parameter);

static void W25Q64_PageProgram_WaitBusyFinished(uint8_t is_success, void *user_parameter);
static void W25Q64_SectorErase_WaitBusyFinished(uint8_t is_success, void *user_parameter);

/**
 * @brief W25Q64初始化函数
 * 
 * @param handle W25Q64句柄指针
 * @param hspi hspix 
 * @param chip_select_gpiox 片选GPIOx
 * @param chip_select_pin 片选Pin
 * @param chip_select_gpio_pinstate 片选gpio激活的状态（选择是高电平有效还是低电平有效）
 */
void W25Q64_Init(W25Q64_Handle_t *handle,SPI_HandleTypeDef *hspi,GPIO_TypeDef *chip_select_gpiox,uint16_t chip_select_pin,GPIO_PinState chip_select_gpio_pinstate)
{
    if (handle == NULL) return;

    handle->hspi = hspi;
    handle->chip_select_gpiox = chip_select_gpiox;
    handle->chip_select_pin = chip_select_pin;
    handle->chip_select_gpio_pinstate = chip_select_gpio_pinstate;

    handle->current_operation = W25Q64_NO_OPERATION;

    handle->read_id_finished_function = NULL;
    handle->user_parameter = NULL;
	handle->write_enable_finished_function = NULL;
	handle->wait_busy_finished_function = NULL;
	handle->page_program_finished_function = NULL;
	handle->sector_erase_finished_function = NULL;
	handle->read_data_finished_function = NULL;

    /* 让回调函数能找到这个 句柄handle */
    W25Q64_Handle_Global = handle;

	SPI_Init(handle->hspi,W25Q64_SPI_TxRxCallback);
}

/**
 * @brief W25Q64读取设备、厂商ID函数
 * 
 * @param handle W25Q64句柄
 * @param finished_function W25Q64_ReadID_FinishedFunction函数
 * @param user_parameter 用户参数指针（可选）
 */
void W25Q64_ReadID(W25Q64_Handle_t *handle,W25Q64_ReadID_FinishedFunction finished_function,void *user_parameter)
{
    if (handle == NULL) return;
    if (handle->hspi == NULL) return;

    /* 记录：当前正在进行“读取 JEDEC ID” */
    handle->current_operation = W25Q64_READING_JEDEC_ID;

    /* 保存用户回调函数与用户参数 */
    handle->read_id_finished_function = finished_function;
    handle->user_parameter = user_parameter;

    /* 准备发送缓冲区：0x9F + 3 个 0xFF（产生时钟换回 3 字节返回值） */
    handle->Tx_buffer[0] = W25Q64_JEDEC_ID;
    handle->Tx_buffer[1] = W25Q64_DUMMY_BYTE;
    handle->Tx_buffer[2] = W25Q64_DUMMY_BYTE;
    handle->Tx_buffer[3] = W25Q64_DUMMY_BYTE;

    /* 接收缓冲区先清零（不是必须，但方便调试） */
    memset(handle->Rx_buffer, 0, 4);

    /* 发起一次“发送并接收”的 DMA 传输：
       命令长度 = 1（0x9F）
       数据长度 = 3（要读回 3 字节） */
    SPI_Transmit_Receive_Data(handle->hspi,handle->chip_select_gpiox,handle->chip_select_pin,handle->chip_select_gpio_pinstate,handle->Tx_buffer,handle->Rx_buffer,1,3);
}

/**
 * @brief W25Q64写使能函数
 * @details 只负责发送 0x06，发送完成后调用 finished_function
 * @param handle W25Q64句柄
 * @param finished_function W25Q64_WriteEnable_FinishedFunction函数（可以为NULL）
 * @param user_parameter 用户参数指针（可选）
 */
void W25Q64_WriteEnable(W25Q64_Handle_t *handle,W25Q64_WriteEnable_FinishedFunction finished_function,void *user_parameter)
{
	if (handle == NULL) return;
    if (handle->hspi == NULL) return;

	/* 记录当前操作 */
    handle->current_operation = W25Q64_WRITING_ENABLE;

	/* 保存回调函数与用户参数 */
    handle->write_enable_finished_function = finished_function;
    handle->user_parameter = user_parameter;

	/* 准备发送缓冲区：只发送 0x06 */
    handle->Tx_buffer[0] = W25Q64_WRITE_ENABLE;

	/* 发起“仅发送”的DMA传输，长度=1 */
    SPI_Transmit_Data(handle->hspi,handle->chip_select_gpiox,handle->chip_select_pin,handle->chip_select_gpio_pinstate,handle->Tx_buffer,1);
}


/**
 * @brief W25Q64等待忙
 * @details 通过反复读取“状态寄存器1”的busy位(bit0)实现
 * @param handle W25Q64句柄
 * @param timeout_ms 超时时间（毫秒）
 * @param finished_function 完成回调函数
 * @param user_parameter 用户参数指针（可选）
 */
void W25Q64_WaitBusy(W25Q64_Handle_t *handle,uint32_t timeout_ms,W25Q64_WaitBusy_FinishedFunction finished_function,void *user_parameter)
{
    if (handle == NULL) return;
    if (handle->hspi == NULL) return;

    /* 记录当前操作 */
    handle->current_operation = W25Q64_WAITING_BUSY;

    /* 保存回调函数与用户参数 */
    handle->wait_busy_finished_function = finished_function;
    handle->user_parameter = user_parameter;

    /* 设置超时截止时间 */
    handle->busy_wait_deadline_tick = HAL_GetTick() + timeout_ms;

     /* 不在这里立刻启动DMA，交给 TaskLoop 统一推进（更稳） */
    handle->read_busy_flag = 1;
    handle->next_read_busy_tick = HAL_GetTick();
}

/**
 * @brief W25Q64任务循环（用于等待忙的轮询推进）
 * @details 建议在APP层循环里周期调用
 * @param handle W25Q64句柄
 */
void W25Q64_TaskLoop(W25Q64_Handle_t *handle)
{
    if (handle == NULL) return;

    if (handle->current_operation != W25Q64_WAITING_BUSY)
    {
        return;
    }

    /* 如果需要继续轮询，并且到达下一次轮询时间，就再发起一次读取状态寄存器1 */
    if (handle->read_busy_flag)
    {
        if ((int32_t)(HAL_GetTick() - handle->next_read_busy_tick) >= 0)
        {
            handle->read_busy_flag = 0;

			/* 命令：0x05 + 1个占位字节，用来换回1字节状态值 */
            handle->Tx_buffer[0] = W25Q64_READ_STATUS_REGISTER_1;
            handle->Tx_buffer[1] = W25Q64_DUMMY_BYTE;

            memset(handle->Rx_buffer, 0, 2);

            SPI_Transmit_Receive_Data(handle->hspi,handle->chip_select_gpiox,handle->chip_select_pin,handle->chip_select_gpio_pinstate,handle->Tx_buffer,handle->Rx_buffer,1,1);
        }
    }
}

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
void W25Q64_PageProgram(W25Q64_Handle_t *handle,uint32_t address, uint8_t *data_array,uint16_t count,uint32_t timeout_ms,W25Q64_PageProgram_FinishedFunction finished_function,void *user_parameter)
{
    uint32_t page_offset;

    if (handle == NULL) return;
    if (handle->hspi == NULL) return;
    if (data_array == NULL) return;

    if (handle->current_operation != W25Q64_NO_OPERATION)
    {
        if (finished_function != NULL)
        {
            finished_function(0, user_parameter);
        }
        return;
    }

    /* 参数检查：长度 1~256 */
    if (count == 0 || count > 256)
    {
        if (finished_function != NULL)
        {
            finished_function(0, user_parameter);
        }
        return;
    }

    /* 参数检查：不能跨页（页大小=256字节） */
    page_offset = (address & 0xFF);
    if ((page_offset + count) > 256)
    {
        if (finished_function != NULL)
        {
            finished_function(0, user_parameter);
        }
        return;
    }

    /* 记录本次页编程信息（异步续接用） */
    handle->page_program_address = address;
    handle->page_program_count = count;
    handle->page_program_timeout_ms = timeout_ms;

    handle->page_program_finished_function = finished_function;
    handle->user_parameter = user_parameter;

    /* 先把数据拷贝到内部缓存（避免上层数组在异步过程中被改动） */
    memcpy(handle->Rx_buffer, data_array, count);

    /* 第一步：发送写使能（写使能完成后续接到页编程发送） */
    W25Q64_WriteEnable(handle, W25Q64_PageProgram_WriteEnableFinished, user_parameter);
}

/**
 * @brief 页编程内部续接：写使能完成 -> 开始发送“页编程指令+地址+数据”
 * 
 * @param user_parameter 用户参数指针（可选）
 */
static void W25Q64_PageProgram_WriteEnableFinished(void *user_parameter)
{
    (void)user_parameter;

    if (W25Q64_Handle_Global == NULL) return;

    /* 组包：0x02 + 3字节地址 + 数据 */
    W25Q64_Handle_Global->Tx_buffer[0] = W25Q64_PAGE_PROGRAM;
    W25Q64_Handle_Global->Tx_buffer[1] = (uint8_t)(W25Q64_Handle_Global->page_program_address >> 16);
    W25Q64_Handle_Global->Tx_buffer[2] = (uint8_t)(W25Q64_Handle_Global->page_program_address >> 8);
    W25Q64_Handle_Global->Tx_buffer[3] = (uint8_t)(W25Q64_Handle_Global->page_program_address);

    /* 数据从内部缓存拷贝到发送缓冲区 */
    memcpy(&W25Q64_Handle_Global->Tx_buffer[4],W25Q64_Handle_Global->Rx_buffer,W25Q64_Handle_Global->page_program_count);

    W25Q64_Handle_Global->current_operation = W25Q64_PAGE_PROGRAM_SENDING;

    /* 仅发送：4字节头 + 数据 */
    SPI_Transmit_Data(W25Q64_Handle_Global->hspi,W25Q64_Handle_Global->chip_select_gpiox,W25Q64_Handle_Global->chip_select_pin,W25Q64_Handle_Global->chip_select_gpio_pinstate,W25Q64_Handle_Global->Tx_buffer,(uint16_t)(4 + W25Q64_Handle_Global->page_program_count));
}

/**
 * @brief 页编程内部续接：页编程等待忙结束 -> 通知用户完成
 * 
 * @param is_success 1表示成功，0表示失败（参数非法 / 超时等）
 * @param user_parameter 用户参数指针（可选）
 */
static void W25Q64_PageProgram_WaitBusyFinished(uint8_t is_success, void *user_parameter)
{
    if (W25Q64_Handle_Global == NULL) return;

    if (W25Q64_Handle_Global->page_program_finished_function != NULL)
    {
        W25Q64_Handle_Global->page_program_finished_function(is_success, user_parameter);
    }
}


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
void W25Q64_SectorErase(W25Q64_Handle_t *handle,uint32_t address,uint32_t timeout_ms,W25Q64_SectorErase_FinishedFunction finished_function,void *user_parameter)
{
    if (handle == NULL) return;
    if (handle->hspi == NULL) return;

    if (handle->current_operation != W25Q64_NO_OPERATION)
    {
        if (finished_function != NULL)
        {
            finished_function(0, user_parameter);
        }
        return;
    }

    handle->sector_erase_address = address;
    handle->sector_erase_timeout_ms = timeout_ms;

    handle->sector_erase_finished_function = finished_function;
    handle->user_parameter = user_parameter;

    /* 第一步：发送写使能（写使能完成后续接到扇区擦除发送） */
    W25Q64_WriteEnable(handle, W25Q64_SectorErase_WriteEnableFinished, user_parameter);
}

/**
 * @brief 扇区擦除内部续接：写使能完成 -> 开始发送“扇区擦除指令+地址”
 * 
 * @param user_parameter 用户参数指针（可选）
 */
static void W25Q64_SectorErase_WriteEnableFinished(void *user_parameter)
{
    (void)user_parameter;

    if (W25Q64_Handle_Global == NULL) return;

    W25Q64_Handle_Global->Tx_buffer[0] = W25Q64_SECTOR_ERASE_4KB;
    W25Q64_Handle_Global->Tx_buffer[1] = (uint8_t)(W25Q64_Handle_Global->sector_erase_address >> 16);
    W25Q64_Handle_Global->Tx_buffer[2] = (uint8_t)(W25Q64_Handle_Global->sector_erase_address >> 8);
    W25Q64_Handle_Global->Tx_buffer[3] = (uint8_t)(W25Q64_Handle_Global->sector_erase_address);

    W25Q64_Handle_Global->current_operation = W25Q64_SECTOR_ERASE_SENDING;

    SPI_Transmit_Data(W25Q64_Handle_Global->hspi,W25Q64_Handle_Global->chip_select_gpiox,W25Q64_Handle_Global->chip_select_pin,W25Q64_Handle_Global->chip_select_gpio_pinstate,W25Q64_Handle_Global->Tx_buffer,4);
}

/**
 * @brief 扇区擦除内部续接：扇区擦除等待忙结束 -> 通知用户完成
 */
static void W25Q64_SectorErase_WaitBusyFinished(uint8_t is_success, void *user_parameter)
{
    if (W25Q64_Handle_Global == NULL) return;

    if (W25Q64_Handle_Global->sector_erase_finished_function != NULL)
    {
        W25Q64_Handle_Global->sector_erase_finished_function(is_success, user_parameter);
    }
}

/**
 * @brief W25Q64读取数据
 * @details
 * 发送流程：
 * 1) 片选有效
 * 2) 发送读取指令0x03 + 3字节地址（共4字节）
 * 3) 继续发送 count 个占位字节0xFF，用于产生时钟并读回数据
 * 4) DMA完成后，在回调函数里把数据从接收缓冲区拷贝到 data_array
 *
 * @param handle W25Q64句柄
 * @param address 读取起始地址
 * @param data_array 接收数据数组指针（必须在回调前保持有效）
 * @param count 读取字节数（建议 <= 256）
 * @param finished_function 完成回调函数
 * @param user_parameter 用户参数指针（可选）
 */
void W25Q64_ReadData(W25Q64_Handle_t *handle,uint32_t address,uint8_t *data_array,uint32_t count,W25Q64_ReadData_FinishedFunction finished_function,void *user_parameter)
{
    uint32_t i;
    uint32_t total_length;

    if (handle == NULL) return;
    if (handle->hspi == NULL) return;
    if (data_array == NULL) return;

    if (handle->current_operation != W25Q64_NO_OPERATION)
    {
        if (finished_function != NULL)
        {
            finished_function(0, data_array, count, user_parameter);
        }
        return;
    }

    /* 由于内部缓冲区为260字节：
       总长度 = 4(指令+地址) + count
       必须 <= 260 */
    total_length = 4 + count;
    if (count == 0 || total_length > sizeof(handle->Tx_buffer))
    {
        if (finished_function != NULL)
        {
            finished_function(0, data_array, count, user_parameter);
        }
        return;
    }

    /* 记录当前操作与回调 */
    handle->current_operation = W25Q64_READING_DATA;

    handle->read_data_finished_function = finished_function;
    handle->user_parameter = user_parameter;

    handle->read_data_address = address;
    handle->read_data_count = count;
    handle->read_data_array = data_array;

    /* 组包发送缓冲区：
       [0]  = 0x03
       [1]  = addr[23:16]
       [2]  = addr[15:8]
       [3]  = addr[7:0]
       [4..]= 0xFF (占位字节，产生时钟读取数据) */
    handle->Tx_buffer[0] = W25Q64_READ_DATA;
    handle->Tx_buffer[1] = (uint8_t)(address >> 16);
    handle->Tx_buffer[2] = (uint8_t)(address >> 8);
    handle->Tx_buffer[3] = (uint8_t)(address);

    for (i = 0; i < count; i++)
    {
        handle->Tx_buffer[4 + i] = W25Q64_DUMMY_BYTE;
    }

    /* 清空接收缓冲区（可选，方便调试） */
    memset(handle->Rx_buffer, 0, total_length);

    /* 发起DMA“发送并接收”
       命令长度 = 4（指令+地址）
       数据长度 = count（要读回的数据） */
    SPI_Transmit_Receive_Data(handle->hspi,handle->chip_select_gpiox,handle->chip_select_pin,handle->chip_select_gpio_pinstate,handle->Tx_buffer,handle->Rx_buffer,4,(uint16_t)count);
}


void W25Q64_SPI_TxRxCallback(uint8_t *Tx_Buffer,uint8_t *Rx_Buffer,uint16_t Tx_Length,uint16_t Rx_Length)
{
    if (W25Q64_Handle_Global == NULL) return;

	//reading id入口
    if (W25Q64_Handle_Global->current_operation == W25Q64_READING_JEDEC_ID)
    {
		if (Rx_Buffer == NULL) return;
        if (Rx_Length != 3) return;

        /* SPI全双工：接收缓冲区第0字节通常对应“发送的命令阶段”，返回数据从索引1开始 */
        uint8_t manufacturer_id = Rx_Buffer[1];
        uint16_t device_id = ((uint16_t)Rx_Buffer[2] << 8) | Rx_Buffer[3];

        /* 当前操作结束 */
        W25Q64_Handle_Global->current_operation = W25Q64_NO_OPERATION;

        /* 调用用户回调函数，把结果交给上层（main） */
        if (W25Q64_Handle_Global->read_id_finished_function != NULL)
        {
            W25Q64_Handle_Global->read_id_finished_function(manufacturer_id,device_id,W25Q64_Handle_Global->user_parameter);
        }
		return;
    }

	/* 写使能完成（仅发送完成，也会进这个回调：Rx_Buffer=NULL, Rx_Length=0） */
	if (W25Q64_Handle_Global->current_operation == W25Q64_WRITING_ENABLE)
    {
        W25Q64_Handle_Global->current_operation = W25Q64_NO_OPERATION;

        if (W25Q64_Handle_Global->write_enable_finished_function != NULL)
        {
            W25Q64_Handle_Global->write_enable_finished_function(W25Q64_Handle_Global->user_parameter);
        }
        return;
    }
	
	/* 页编程“发送帧”完成：进入等待忙 */
    if (W25Q64_Handle_Global->current_operation == W25Q64_PAGE_PROGRAM_SENDING)
    {
        W25Q64_Handle_Global->current_operation = W25Q64_NO_OPERATION;

        /* 进入等待忙：等待写入真正完成 */
        W25Q64_WaitBusy(W25Q64_Handle_Global,W25Q64_Handle_Global->page_program_timeout_ms,W25Q64_PageProgram_WaitBusyFinished,W25Q64_Handle_Global->user_parameter);
        return;
    }

	 /* 扇区擦除“发送帧”完成：进入等待忙 */
    if (W25Q64_Handle_Global->current_operation == W25Q64_SECTOR_ERASE_SENDING)
    {
        W25Q64_Handle_Global->current_operation = W25Q64_NO_OPERATION;

        W25Q64_WaitBusy(W25Q64_Handle_Global,W25Q64_Handle_Global->sector_erase_timeout_ms,W25Q64_SectorErase_WaitBusyFinished,W25Q64_Handle_Global->user_parameter);
        return;
    }

	/* 等待忙：每次读完状态寄存器1都在这里判断 busy 位 */
    if (W25Q64_Handle_Global->current_operation == W25Q64_WAITING_BUSY)
    {
        if (Rx_Buffer == NULL) return;
        if (Rx_Length != 1) return;

        /* 超时判断 */
        if ((int32_t)(HAL_GetTick() - W25Q64_Handle_Global->busy_wait_deadline_tick) > 0)
        {
            W25Q64_Handle_Global->current_operation = W25Q64_NO_OPERATION;

            if (W25Q64_Handle_Global->wait_busy_finished_function != NULL)
            {
                W25Q64_Handle_Global->wait_busy_finished_function(0, W25Q64_Handle_Global->user_parameter);
            }
            return;
        }

         /* 状态寄存器1的busy位是 bit0，返回数据在索引1 */
        uint8_t status_register_1 = Rx_Buffer[1];
        if ((status_register_1 & 0x01) != 0)
        {
            /* 还在忙：不要在中断里立刻再发DMA，设置标志，交给 TaskLoop 再发 */
            W25Q64_Handle_Global->read_busy_flag = 1;
            W25Q64_Handle_Global->next_read_busy_tick = HAL_GetTick() + 1; /* 间隔1ms再读一次 */
        }
        else
        {
            /* 不忙：等待成功 */
            W25Q64_Handle_Global->current_operation = W25Q64_NO_OPERATION;

            if (W25Q64_Handle_Global->wait_busy_finished_function != NULL)
            {
                W25Q64_Handle_Global->wait_busy_finished_function(1, W25Q64_Handle_Global->user_parameter);
            }
        }
        return;
    }

	/* 读取数据完成 */
	if (W25Q64_Handle_Global->current_operation == W25Q64_READING_DATA)
	{
		uint32_t i;

		if (Rx_Buffer == NULL) return;
		/* Rx_Length 应该等于 count（你SPI库传进来的“数据长度”） */
		if (Rx_Length != (uint16_t)W25Q64_Handle_Global->read_data_count) return;

		/* 返回数据从接收缓冲区索引4开始（前4字节是指令+地址阶段的回读垃圾） */
		for (i = 0; i < W25Q64_Handle_Global->read_data_count; i++)
		{
			W25Q64_Handle_Global->read_data_array[i] = Rx_Buffer[4 + i];
		}

		W25Q64_Handle_Global->current_operation = W25Q64_NO_OPERATION;

		if (W25Q64_Handle_Global->read_data_finished_function != NULL)
		{
			W25Q64_Handle_Global->read_data_finished_function(1,W25Q64_Handle_Global->read_data_array,W25Q64_Handle_Global->read_data_count,W25Q64_Handle_Global->user_parameter);
		}
		return;
	}
}
