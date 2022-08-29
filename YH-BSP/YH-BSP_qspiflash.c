#include "YH-BSP_qspiflash.h"

extern QSPI_HandleTypeDef hqspi;

/* 僅供內部使用 */
static uint8_t bsp_QSPI_Flash_EnterFourBytesAddress(void);
static uint8_t bsp_QSPI_Flash_ExitFourBytesAddress(void);
static uint8_t bsp_QSPI_Flash_AutoPollingMemReady(uint32_t Timeout);
static uint8_t bsp_QSPI_Flash_WriteEnable(void);
static uint32_t bsp_QSPI_Flash_WriteStatusReg(uint8_t reg, uint8_t regvalue);
static uint32_t bsp_QSPI_Flash_ReadStatusReg(uint8_t reg);
static void bsp_QSPI_Flash_Set_WP_High(void);
static void bsp_QSPI_Flash_Set_WP_TO_QSPI_IO(void);

/**
 * @brief  返回QSPI存储器的配置
 * @param  pInfo: 在配置结构上的指针
 * @retval QSPI存储器状态
 */
uint8_t bsp_QSPI_Flash_GetInfo(QSPI_Info *pInfo)
{
	/* 配置存储器配置结构 */
	pInfo->FlashSize = W25Q256JV_FLASH_SIZE;
	pInfo->EraseSectorSize = W25Q256JV_SECTOR_SIZE;
	pInfo->EraseSectorsNumber = (W25Q256JV_FLASH_SIZE / W25Q256JV_SECTOR_SIZE);
	pInfo->ProgPageSize = W25Q256JV_PAGE_SIZE;
	pInfo->ProgPagesNumber = (W25Q256JV_FLASH_SIZE / W25Q256JV_PAGE_SIZE);
	return QSPI_OK;
}

/**
 * @brief 配置QSPI為映射模式
 * @return uint8_t QSPI狀態
 */
uint8_t bsp_QSPI_Flash_EnableMemoryMappedMode(void)
{
	QSPI_CommandTypeDef s_command = {0};
	QSPI_MemoryMappedTypeDef s_mem_mapped_cfg = {0};

	/* 重從新配置QSPI控制器 */
	HAL_QSPI_DeInit(&hqspi);
	hqspi.Instance = QUADSPI;
	hqspi.Init.ClockPrescaler = 1;
	hqspi.Init.FifoThreshold = 32;
	hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
	hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_2_CYCLE; // MIN:10ns 1/120=8.3 8.3*2
	hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
#if (DUALFLASH == 0)
	hqspi.Init.FlashSize = 25 - 1;
	hqspi.Init.FlashID = QSPI_FLASH_ID_1;
	hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
#elif (DUALFLASH == 1)
	hqspi.Init.FlashSize = 26 - 1;
	//hqspi.Init.FlashID = QSPI_FLASH_ID_1;
	hqspi.Init.DualFlash = QSPI_DUALFLASH_ENABLE;
#endif
	if (HAL_QSPI_Init(&hqspi) != HAL_OK)
	{
		Error_Handler();
	}

	/* 基本配置 */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* 4線配置 */
	s_command.Instruction = QUAD_INOUT_FAST_READ_CMD_4BYTE;
	s_command.AddressMode = QSPI_ADDRESS_4_LINES;
	s_command.DataMode = QSPI_DATA_4_LINES;
	s_command.DummyCycles = 6;

	/* 配置映射模式 */
	s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
	s_mem_mapped_cfg.TimeOutPeriod = 0;

	if (HAL_QSPI_MemoryMapped(&hqspi, &s_command, &s_mem_mapped_cfg) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/**
 * @brief 離開QSPI映射模式
 * @return uint8_t QSPI狀態
 */
uint8_t bsp_QSPI_Flash_QuitMemoryMappedMode(void)
{
	if (HAL_QSPI_Abort(&hqspi) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
 * @brief 進入QSPI存儲器為4-byte地址模式
 * @return uint8_t QSPI狀態
 */
static uint8_t bsp_QSPI_Flash_EnterFourBytesAddress(void)
{
	QSPI_CommandTypeDef s_command = {0};

	/* 基本配置 */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* 其他配置 */
	s_command.Instruction = ENTER_4_BYTE_ADDR_MODE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0;

	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 配置自動輪詢模式等待存儲器就緒 */
	if (bsp_QSPI_Flash_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
 * @brief 離開QSPI存儲器為4-byte地址模式
 * @return uint8_t QSPI狀態
 */
static uint8_t bsp_QSPI_Flash_ExitFourBytesAddress(void)
{
	QSPI_CommandTypeDef s_command = {0};

	/* 基本配置 */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* 其他配置 */
	s_command.Instruction = EXIT_4_BYTE_ADDR_MODE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0;

	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 配置自動輪詢模式等待存儲器就緒 */
	if (bsp_QSPI_Flash_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
 * @brief  初始化QSPI存储器
 * @retval QSPI存储器状态
 */
uint8_t bsp_QSPI_Flash_Init(void)
{
	/* 使能四通道IO2和IO3引腳 */
	bsp_QSPI_Flash_WriteStatusReg(1, W25Q256JV_FSR_QE);

	/* QSPI memory reset */
	if (bsp_QSPI_Flash_ResetMemory() != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	/* 進入四線模式 */
	bsp_QSPI_Flash_EnterFourBytesAddress();

	return QSPI_OK;
}

/**
 * @brief 從QSPI FLASH中讀取資料(1線)
 * @param pData 指向要讀取得資料的指針
 * @param ReadAddr 要讀取的FLASH首地址
 * @param Size 資料大小
 * @return uint8_t QSPI狀態
 */
uint8_t bsp_QSPI_Flash_Read(uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
	QSPI_CommandTypeDef s_command = {0};

	if (Size == 0)
	{
		printf("bsp_QSPI_Flash_Read Size = 0");
		return QSPI_OK;
	}

	/* 基本配置 */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* 其他配置 */
	s_command.Instruction = READ_CMD_4BYTE;
	s_command.AddressMode = QSPI_ADDRESS_1_LINE;
	s_command.Address = ReadAddr;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.NbData = Size;
	s_command.DummyCycles = 0;

	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
 * @brief 從QSPI FLASH中讀取資料(4線)
 * @param pData 指向要讀取得資料的指針
 * @param ReadAddr 要讀取的首地址
 * @param Size 資料大小
 * @return uint8_t QSPI狀態
 */
uint8_t bsp_QSPI_Flash_FastRead(uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
	QSPI_CommandTypeDef s_command = {0};

	if (Size == 0)
	{
		printf("bsp_QSPI_Flash_FastRead Size = 0");
		return QSPI_OK;
	}
	/* 基本配置 */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* 其他配置 */
	s_command.Instruction = QUAD_INOUT_FAST_READ_CMD_4BYTE;
	s_command.AddressMode = QSPI_ADDRESS_4_LINES;
	s_command.Address = ReadAddr;
	s_command.DataMode = QSPI_DATA_4_LINES;
	s_command.NbData = Size;
	s_command.DummyCycles = 6;

	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
 * @brief 寫入資料到QSPI FLASH中
 * @param pData 指向要寫入資料的指針
 * @param WriteAddr 要寫到FLASH的首地址
 * @param Size 資料大小
 * @return uint8_t QSPI狀態
 */
uint8_t bsp_QSPI_Flash_Write(uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
	QSPI_CommandTypeDef s_command = {0};
	uint32_t end_addr, current_size, current_addr;

	if (Size == 0)
	{
		printf("bsp_QSPI_Flash_Write Size = 0");
		return QSPI_OK;
	}

	/* 記涮寫入地址和頁面末尾之間的大小 */
	current_addr = 0;
	while (current_addr <= WriteAddr)
	{
		current_addr += W25Q256JV_PAGE_SIZE;
	}
	current_size = current_addr - WriteAddr;

	/* 檢查資料的大小是否小于頁面中的剩餘位置 */
	if (current_size > Size)
	{
		current_size = Size;
	}
	/* 初始化地址變數 */
	current_addr = WriteAddr;
	end_addr = WriteAddr + Size;

	/* 基本配置 */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* 其他配置 */
	s_command.Instruction = QUAD_INPUT_PAGE_PROG_CMD_4BYTE;
	s_command.AddressMode = QSPI_ADDRESS_1_LINE;
	s_command.DataMode = QSPI_DATA_4_LINES;
	s_command.DummyCycles = 0;

	/* 逐頁寫入 */
	do
	{
		s_command.Address = current_addr;
		if (current_size == 0)
		{
			printf("bsp_QSPI_Flash_Write current_size = 0");
			return QSPI_OK;
		}

		s_command.NbData = current_size;

		/* 啟用寫操作 */
		if (bsp_QSPI_Flash_WriteEnable() != QSPI_OK)
		{
			return QSPI_ERROR;
		}

		if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		{
			return QSPI_ERROR;
		}

		if (HAL_QSPI_Transmit(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		{
			return QSPI_ERROR;
		}

		if (bsp_QSPI_Flash_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
		{
			return QSPI_ERROR;
		}

		/* 更新下一頁編程的地址和大小變數 */
		current_addr += current_size;
		pData += current_size;
		current_size = ((current_addr + W25Q256JV_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q256JV_PAGE_SIZE;
	} while (current_addr < end_addr);
	return QSPI_OK;
}

/**
 * @brief 擦除QSPI Flash的指定扇區
 * @param SectorAddress 需要擦除的扇區地址
 * @return uint8_t QSPI狀態
 */
uint8_t bsp_QSPI_Flash_Erase_Sector(uint32_t SectorAddress)
{
	QSPI_CommandTypeDef s_command = {0};

	/* 基本配置 */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* 其他配置 */
	s_command.Instruction = SECTOR_ERASE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_1_LINE;
	s_command.Address = SectorAddress;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.NbData = 0;
	s_command.DummyCycles = 0;

	/* 啟用寫操作 */
	if (bsp_QSPI_Flash_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	/* 發送命令 */
	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 配置自動輪詢模式等待擦除结束 */
	if (bsp_QSPI_Flash_AutoPollingMemReady(W25Q256JV_SECTOR_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
 * @brief 擦除QSPI Flash的指定塊，單塊大小32k
 * @param BlockAddress 需要擦除的塊地址
 * @return uint8_t QSPI狀態
 */
uint8_t bsp_QSPI_Flash_Erase_Block32(uint32_t BlockAddress)
{
	QSPI_CommandTypeDef s_command = {0};

	/* 基本配置 */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* 其他配置 */
	s_command.Instruction = BLOCK32_ERASE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_1_LINE;
	s_command.Address = BlockAddress;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.NbData = 0;
	s_command.DummyCycles = 0;

	/* 啟用寫操作 */
	if (bsp_QSPI_Flash_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	/* 發送命令 */
	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 配置自動輪詢模式等待擦除结束 */
	if (bsp_QSPI_Flash_AutoPollingMemReady(W25Q256JV_SECTOR_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
 * @brief 擦除QSPI Flash的指定塊，單塊大小64k
 * @param BlockAddress 需要擦除的塊地址
 * @return uint8_t QSPI狀態
 */
uint8_t bsp_QSPI_Flash_Erase_Block64(uint32_t BlockAddress)
{
	QSPI_CommandTypeDef s_command = {0};
	/* 基本配置 */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* 其他配置 */
	s_command.Instruction = BLOCK64_ERASE_CMD_4BYTE;
	s_command.AddressMode = QSPI_ADDRESS_1_LINE;
	s_command.Address = BlockAddress;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.NbData = 0;
	s_command.DummyCycles = 0;

	/* 啟用寫操作 */
	if (bsp_QSPI_Flash_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	/* 發送命令 */
	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 配置自動輪詢模式等待擦除结束 */
	if (bsp_QSPI_Flash_AutoPollingMemReady(W25Q256JV_SECTOR_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
 * @brief 擦除整個QSPI Flash
 * @return uint8_t QSPI狀態
 */
uint8_t bsp_QSPI_Flash_Erase_Chip(void)
{
	QSPI_CommandTypeDef s_command = {0};

	/* 基本配置 */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* 其他配置 */
	s_command.Instruction = CHIP_ERASE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.NbData = 0;
	s_command.DummyCycles = 0;

	/* 啟用寫操作 */
	if (bsp_QSPI_Flash_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	/* 發送命令 */
	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	/* 配置自動輪詢模式等待擦除结束 */
	if (bsp_QSPI_Flash_AutoPollingMemReady(W25Q256JV_BULK_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
 * @brief 復位QSPI Flash
 * @return uint8_t QSPI狀態
 */
uint8_t bsp_QSPI_Flash_ResetMemory()
{
	QSPI_CommandTypeDef s_command = {0};
	/* 基本配置 */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/*其他配置*/
	s_command.Instruction = RESET_ENABLE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0;

	/* 發送命令 */
	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 发送复位存储器命令 */
	s_command.Instruction = RESET_MEMORY_CMD;
	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 配置自動輪詢模式等待存儲器就緒 */
	if (bsp_QSPI_Flash_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
 * @brief 發送寫入使能，等待它有效.
 * @return uint8_t QSPI狀態
 */
static uint8_t bsp_QSPI_Flash_WriteEnable()
{
	QSPI_CommandTypeDef s_command = {0};
	QSPI_AutoPollingTypeDef s_config = {0};
	/* 基本配置 */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/*其他配置*/
	s_command.Instruction = WRITE_ENABLE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0;

	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 配置自動輪詢模式等待寫啟用 */
	s_config.Match = W25Q256JV_FSR_WREN;
	s_config.Mask = W25Q256JV_FSR_WREN;
	s_config.MatchMode = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 2;
	s_config.Interval = 0x10;
	s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

	s_command.Instruction = READ_STATUS_REG1_CMD;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.NbData = 1;

	if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
 * @brief  读取存储器的SR并等待EOP
 * @param  hqspi: QSPI句柄
 * @param  Timeout 超时
 * @retval 无
 */
static uint8_t bsp_QSPI_Flash_AutoPollingMemReady(uint32_t Timeout)
{
	QSPI_CommandTypeDef s_command = {0};
	QSPI_AutoPollingTypeDef s_config = {0};

	/* 配置自動輪詢模式等待存儲器準備就緒 */
	/* 基本配置 */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* 其他配置 */
	s_command.Instruction = READ_STATUS_REG1_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.NbData = 0;
	s_command.DummyCycles = 0;

	s_config.Mask = W25Q256JV_FSR_BUSY;
	s_config.Match = 0x00;
	s_config.MatchMode = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 1;
	s_config.Interval = 0x10;
	s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

	if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, Timeout) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
 * @brief 讀取FLASH ID
 * @return uint32_t FLASH ID
 */
uint32_t bsp_QSPI_Flash_ReadID(void)
{
	QSPI_CommandTypeDef s_command = {0};
	uint32_t _id = 0;
	uint8_t pData[6];

	/* 基本配置 */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* 其他配置 */
	s_command.Instruction = READ_JEDEC_ID_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0;
	s_command.NbData = 6;

	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 可以在這裡添加一些代碼來處理這個錯誤 */
		while (1)
		{
		}
	}
	if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 可以在這裡添加一些代碼來處理這個錯誤 */
		while (1)
		{
		}
	}

	_id = (pData[0] << 16) | (pData[2] << 8) | (pData[4]);

	return _id;
}

/**
 * @brief 讀取FLASH Device ID
 * @return uint32_t FLASH Device ID
 */
uint32_t bsp_QSPI_Flash_ReadDeviceID(void)
{
	QSPI_CommandTypeDef s_command = {0};
	uint32_t Temp = 0;
	uint8_t pData[3];

	/* 基本配置 */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* 其他配置 */
	s_command.Instruction = READ_ID_CMD;
	s_command.AddressMode = QSPI_ADDRESS_1_LINE;
	s_command.Address = 0x000000;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0;
	s_command.NbData = 3;

	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 可以在這裡添加一些代碼來處理這個錯誤 */
		while (1)
		{
		}
	}
	if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 可以在這裡添加一些代碼來處理這個錯誤 */
		while (1)
		{
		}
	}

	Temp = pData[0] | (pData[2] << 8);

	return Temp;
}

/**
 * @brief 讀取StatusReg
 * @param reg 狀態暫存器
 * @return uint32_t QSPI狀態
 */
static uint32_t bsp_QSPI_Flash_ReadStatusReg(uint8_t reg)
{
	QSPI_CommandTypeDef s_command = {0};
	uint32_t Temp = 0;
	uint8_t pData[10];

	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	if (reg == 1)
		s_command.Instruction = READ_STATUS_REG1_CMD;
	else if (reg == 2)
		s_command.Instruction = READ_STATUS_REG2_CMD;
	else if (reg == 3)
		s_command.Instruction = READ_STATUS_REG3_CMD;

	s_command.AddressMode = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_32_BITS;
	s_command.Address = 0x000000;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0;
	s_command.NbData = 1;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while (1)
		{
		}
	}
	if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while (1)
		{
		}
	}

	//单flash时读取的字节数为pData[0]，pData[1]
	// Temp = pData[0] |( pData[1]<<8 ) ;

	//双flash时读取的字节数为pData[0]，pData[2]
	// Temp = pData[1] |( pData[0]<<8 ) ;
	Temp = pData[0];
	return Temp;
}

/**
 * @brief 寫入StatusReg
 * @param reg 狀態暫存器
 * @param regvalue 狀態標誌
 * @return uint32_t QSPI狀態
 */
static uint32_t bsp_QSPI_Flash_WriteStatusReg(uint8_t reg, uint8_t regvalue)
{
	QSPI_CommandTypeDef s_command = {0};

	if (bsp_QSPI_Flash_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	if (reg == 1)
		s_command.Instruction = WRITE_STATUS_REG1_CMD;
	else if (reg == 2)
		s_command.Instruction = WRITE_STATUS_REG2_CMD;
	else if (reg == 3)
		s_command.Instruction = WRITE_STATUS_REG3_CMD;

	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AddressSize = QSPI_ADDRESS_8_BITS;
	s_command.Address = 0x000000;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0;
	s_command.NbData = 1;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while (1)
		{
		}
	}
	if (HAL_QSPI_Transmit(&hqspi, &regvalue, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while (1)
		{
		}
	}
	/* 配置自動輪詢模式等待存儲器就緒 */
	if (bsp_QSPI_Flash_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

static void bsp_QSPI_Flash_Set_WP_High(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__GPIOF_CLK_ENABLE();
	/*选择要控制的GPIO引脚*/
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO2_Pin;
	/*设置引脚的输出类型为推挽输出*/
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	/*设置引脚为上拉模式*/
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	/*设置引脚速率为高速 */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	/*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO2_GPIO_Port, &GPIO_InitStruct);

	HAL_GPIO_WritePin(QSPI_FLASH_BK1_IO2_GPIO_Port, QSPI_FLASH_BK1_IO2_Pin, GPIO_PIN_SET);
}
static void bsp_QSPI_Flash_Set_WP_TO_QSPI_IO(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__GPIOF_CLK_ENABLE();

	//设置引脚
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	/*!< 配置 QSPI_FLASH 引脚: IO2 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO2_Pin;
	GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO2_GPIO_Port, &GPIO_InitStruct);
}

/*******************************************************************************************************/
/*測試用*/
bsp_Status bsp_QSPI_Flash_Test(void)
{
	//__IO bsp_STATUS transferstatus = FAILED;
	__IO uint8_t *qspi_addr = (__IO uint8_t *)(0x90000000);
#define countof(a) (sizeof(a) / sizeof(*(a)))
#define BufferSize (countof(Tx_Buffer) - 1)
	uint8_t Tx_Buffer[] = "STM32H750XB_Roy_Project_QSPI_DualFlash_Test.";
	uint8_t Rx_Buffer[BufferSize];
	uint8_t Rx_Buffer2[BufferSize];
	uint32_t addr = 0x93000000;
	int state = QSPI_ERROR;

	printf("sFLASH_ID:0x%x\r\n", bsp_QSPI_Flash_ReadID());
	printf("ReadDeviceID:0x%x\r\n", bsp_QSPI_Flash_ReadDeviceID());

	state = bsp_QSPI_Flash_Erase_Sector(addr);
	if (state == QSPI_OK)
		printf("\r\n擦除成功!\r\n");
	else
	{
		printf("\r\n擦除失敗!\r\n");
		while (1)
			;
	}
	printf("\r\n正在向%d地址寫入，大小為%d!\r\n", addr, BufferSize);
	/* 將發送緩衝區的數據寫到flash中 */
	bsp_QSPI_Flash_Write(Tx_Buffer, addr, BufferSize);
	printf("\r\n寫入成功!\r\n");
	bsp_QSPI_Flash_FastRead(Rx_Buffer, addr, BufferSize);
	printf("Rx_Buffer: %s\r\n", Rx_Buffer);

	/* QSPI memory reset */
	if (bsp_QSPI_Flash_ResetMemory() != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	if (bsp_QSPI_Flash_EnableMemoryMappedMode() != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	printf("\r\n---使用memcpy讀取QPSI的内容----\n\r");
	memcpy(Rx_Buffer2, (uint8_t *)qspi_addr, BufferSize);

	/* 檢查寫入的數據與讀出的數據是否相等 */
	state = bsp_Buffercmp(Tx_Buffer, Rx_Buffer2, BufferSize);
	if (QSPI_OK == state)
	{
		printf("\r\n讀寫(memcpy)測試成功!\n\r");
	}
	else
	{
		printf("\r\n讀寫(memcpy)測試失敗!\n\r");
	}
	printf("\r\n-------使用指針讀取QPSI的内容-------\n\r");

	memset(Rx_Buffer2, 0, BufferSize);

	for (int i = 0; i < BufferSize; i++)
	{
		Rx_Buffer2[i] = *qspi_addr;
		qspi_addr++;
	}
	/* 檢查寫入的數據與讀出的數據是否相等 */
	state = bsp_Buffercmp(Tx_Buffer, Rx_Buffer2, BufferSize);

	if (QSPI_OK == state)
	{
		printf("\r\n讀寫(指針操作)測試成功!\n\r");
	}
	else
	{
		printf("\r\n讀寫(指針操作)測試失敗!\n\r");
	}
	printf("Rx_Buffer2: %s\r\n", Rx_Buffer2);

	if (state == QSPI_OK)
	{
		return PASSED;
	}
	else
	{
		return FAILED;
	}
}
/*********************************************END OF FILE**********************/
