#include "YH-BSP_FMC_sdram.h"
extern SDRAM_HandleTypeDef hsdram2;
#define sdramHandle hsdram2

/**
 * @brief SDRAM初始化配置
 * @param hsdram SDRAM_Handle
 * @param Command FMC_SDRAM_Command
 */
void bsp_SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command)
{
  __IO uint32_t tmpr = 0;

  /* Step 1 ----------------------------------------------------------------*/
  /* 配置命令：開啟提供給SDRAM的時鐘 */
  Command->CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
  Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
  Command->AutoRefreshNumber = 1;
  Command->ModeRegisterDefinition = 0;
  /* 发送配置命令 */
  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

  /* Step 2: delay 100us */
  HAL_Delay(100);

  /* Step 3 ----------------------------------------------------------------*/
  /* 配置命令：對所有的bank預充電 */
  Command->CommandMode = FMC_SDRAM_CMD_PALL;
  Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
  Command->AutoRefreshNumber = 1;
  Command->ModeRegisterDefinition = 0;
  /* 發送配置命令 */
  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

  /* Step 4 ----------------------------------------------------------------*/
  /* 配置命令：自動刷新 */
  Command->CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
  Command->AutoRefreshNumber = 8;
  Command->ModeRegisterDefinition = 0;
  /* 發送配置命令 */
  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

  /* Step 5 ----------------------------------------------------------------*/
  /* 配置sdram暫存器配置 */
  tmpr = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1 |
         SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
         SDRAM_MODEREG_CAS_LATENCY_3 |
         SDRAM_MODEREG_OPERATING_MODE_STANDARD |
         SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

  /* 配置命令：設置SDRAM暫存器 */
  Command->CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
  Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
  Command->AutoRefreshNumber = 1;
  Command->ModeRegisterDefinition = tmpr;
  /* 發送配置命令 */
  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

  /* Step 6 ----------------------------------------------------------------*/

  /* 設置刷新計數器 */
  /* SDRAM refresh period / Number of rows）*SDRAM clock – 20 */
  /* 刷新周期=64ms/8192 = 7.8125us */
  /* COUNT=(7.8125us x Freq) - 20 */
  /* 設置自刷新速率 */
  HAL_SDRAM_ProgramRefreshRate(hsdram, 916);
}

/**
 * @brief 向sdram寫入資料
 * @param pBuffer 指向資料的指针
 * @param uwWriteAddress SDRAM WriteAddress
 * @param uwBufferSize Data Size
 */
void bsp_SDRAM_WriteBuffer(uint32_t *pBuffer, uint32_t uwWriteAddress, uint32_t uwBufferSize)
{
  __IO uint32_t write_pointer = (uint32_t)uwWriteAddress;
  if (write_pointer >= SDRAM_SIZE)
    /* 禁止寫保護 */
    HAL_SDRAM_WriteProtection_Disable(&sdramHandle);
  for (; uwBufferSize != 0; uwBufferSize--)
  {
    *(uint32_t *)(SDRAM_BANK_ADDR + write_pointer) = *pBuffer++;
    write_pointer += 4;
  }
}

/**
 * @brief 向sdram讀取資料
 * @param pBuffer 指向資料的指针
 * @param uwReadAddress SDRAM ReadAddress
 * @param uwBufferSize Data Size
 */
void bsp_SDRAM_ReadBuffer(uint32_t *pBuffer, uint32_t uwReadAddress, uint32_t uwBufferSize)
{
  __IO uint32_t write_pointer = (uint32_t)uwReadAddress;
  for (; uwBufferSize != 0; uwBufferSize--)
  {
    *pBuffer++ = *(__IO uint32_t *)(SDRAM_BANK_ADDR + write_pointer);
    write_pointer += 4;
  }
}

/**
  * @brief  测试SDRAM是否正常
  * @param  None
  * @retval 正常返回1，异常返回0
  */
uint8_t bsp_SDRAM_Test(void)
{
  /*写入数据计数器*/
  uint32_t counter = 0;

  /* 8位的数据 */
  uint8_t ubWritedata_8b = 0, ubReaddata_8b = 0;

  /* 16位的数据 */
  uint16_t uhWritedata_16b = 0, uhReaddata_16b = 0;

  /* 32位的数据 */
  uint32_t uhWritedata_32b = 0, uhReaddata_32b = 0;

  SDRAM_INFO("SDRAM ,8bit,16bit,32bit testsdram...");

  /*按8位格式读写数据，并校验*/

  /* 把SDRAM数据全部重置为0 ，SDRAM_SIZE是以8位为单位的 */
  for (counter = 0x00; counter < SDRAM_SIZE; counter++)
  {
    *(__IO uint8_t *)(SDRAM_BANK_ADDR + counter) = (uint8_t)0x0;
  }

  /* 向整个SDRAM写入数据  8位 */
  for (counter = 0; counter < SDRAM_SIZE; counter++)
  {
    *(__IO uint8_t *)(SDRAM_BANK_ADDR + counter) = (uint8_t)(ubWritedata_8b + counter);
  }

  /* 读取 SDRAM 数据并检测*/
  for (counter = 0; counter < SDRAM_SIZE; counter++)
  {
    ubReaddata_8b = *(__IO uint8_t *)(SDRAM_BANK_ADDR + counter); //从该地址读出数据

    if (ubReaddata_8b != (uint8_t)(ubWritedata_8b + counter)) //检测数据，若不相等，跳出函数,返回检测失败结果。
    {
      SDRAM_ERROR("8bit RW error! Location:%d", counter);
      return 0;
    }
  }

  /*按16位格式读写数据，并检测*/

  /* 把SDRAM数据全部重置为0 */
  for (counter = 0x00; counter < SDRAM_SIZE / 2; counter++)
  {
    *(__IO uint16_t *)(SDRAM_BANK_ADDR + 2 * counter) = (uint16_t)0x00;
  }

  /* 向整个SDRAM写入数据  16位 */
  for (counter = 0; counter < SDRAM_SIZE / 2; counter++)
  {
    *(__IO uint16_t *)(SDRAM_BANK_ADDR + 2 * counter) = (uint16_t)(uhWritedata_16b + counter);
  }

  /* 读取 SDRAM 数据并检测*/
  for (counter = 0; counter < SDRAM_SIZE / 2; counter++)
  {
    uhReaddata_16b = *(__IO uint16_t *)(SDRAM_BANK_ADDR + 2 * counter); //从该地址读出数据

    if (uhReaddata_16b != (uint16_t)(uhWritedata_16b + counter)) //检测数据，若不相等，跳出函数,返回检测失败结果。
    {
      SDRAM_ERROR("16bit RW error! Location:%d", counter);

      return 0;
    }
  }

  /*按32位格式读写数据，并检测*/

  /* 把SDRAM数据全部重置为0 */
  for (counter = 0x00; counter < SDRAM_SIZE / 4; counter++)
  {
    *(__IO uint32_t *)(SDRAM_BANK_ADDR + 4 * counter) = (uint32_t)0x00;
  }

  /* 向整个SDRAM写入数据  32位 */
  for (counter = 0; counter < SDRAM_SIZE / 4; counter++)
  {
    *(__IO uint32_t *)(SDRAM_BANK_ADDR + 4 * counter) = (uint32_t)(uhWritedata_32b + counter);
  }

  /* 读取 SDRAM 数据并检测*/
  for (counter = 0; counter < SDRAM_SIZE / 4; counter++)
  {
    uhReaddata_32b = *(__IO uint32_t *)(SDRAM_BANK_ADDR + 4 * counter); //从该地址读出数据

    if (uhReaddata_32b != (uint32_t)(uhWritedata_32b + counter)) //检测数据，若不相等，跳出函数,返回检测失败结果。
    {
      SDRAM_ERROR("32bit RW error! Location:%d", counter);

      return 0;
    }
  }

  SDRAM_INFO("SDRAM RW Test OK!");
  /*測試正常，return 1 */
  return 1;
}

/**
 * @brief 掃描測試外部SDRAM的全部單元
 * @return uint32_t 0 通過 ; 大於0表示錯誤單元數
 */
uint32_t bsp_SDRAM_Test2(void)
{
  uint32_t i;
  uint32_t *pSRAM;
  uint8_t *pBytes;
  uint32_t err;
  const uint8_t ByteBuf[4] = {0x55, 0xA5, 0x5A, 0xAA};

  /* Write */
  pSRAM = (uint32_t *)SDRAM_BANK_ADDR;
  for (i = 0; i < SDRAM_SIZE / 4; i++)
  {
    *pSRAM++ = i;
  }

  /* Read */
  err = 0;
  pSRAM = (uint32_t *)SDRAM_BANK_ADDR;
  for (i = 0; i < SDRAM_SIZE / 4; i++)
  {
    if (*pSRAM++ != i)
    {
      err++;
    }
  }

  if (err > 0)
  {
    return (4 * err);
  }

  /* 對SDRAM的DATA求反並寫入 */
  pSRAM = (uint32_t *)SDRAM_BANK_ADDR;
  for (i = 0; i < SDRAM_SIZE / 4; i++)
  {
    *pSRAM = ~*pSRAM;
    pSRAM++;
  }

  /* 再次比較SDRAM的DATA */
  err = 0;
  pSRAM = (uint32_t *)SDRAM_BANK_ADDR;
  for (i = 0; i < SDRAM_SIZE / 4; i++)
  {
    if (*pSRAM++ != (~i))
    {
      err++;
    }
  }

  if (err > 0)
  {
    return (4 * err);
  }

  /*測試按Byte方式訪問,目的是驗證FMC_NBL0,FMC_NBL1*/
  pBytes = (uint8_t *)SDRAM_BANK_ADDR;
  for (i = 0; i < sizeof(ByteBuf); i++)
  {
    *pBytes++ = ByteBuf[i];
  }

  /* 比較SDRAM的DATA */
  err = 0;
  pBytes = (uint8_t *)SDRAM_BANK_ADDR;
  for (i = 0; i < sizeof(ByteBuf); i++)
  {
    if (*pBytes++ != ByteBuf[i])
    {
      err++;
    }
  }
  if (err > 0)
  {
    return err;
  }
  return 0;
}

/**
 * @brief 寫SDRAM速度測試(64MB)
 */
void bsp_WriteSpeedTest(void)
{
  uint32_t i, j;
  int32_t iTime1, iTime2;
  uint32_t *pBuf;

  j = 0;
  pBuf = (uint32_t *)SDRAM_BANK_ADDR;
  iTime1 = bsp_GetRunTime(); //記錄開始時間
  /*以遞增的方式寫資料到SDRAM所有空間*/
  for (i = 1024 * 1024 / 4; i > 0; i--)
  {
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;

    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;

    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;

    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;

    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;

    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;

    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;

    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
    *pBuf++ = j++;
  }

  iTime2 = bsp_GetRunTime(); /* 紀錄結束時間 */
  /* 讀取寫入的是否出錯 */
  j = 0;
  pBuf = (uint32_t *)SDRAM_BANK_ADDR;
  for (i = 0; i < 1024 * 1024 * 8; i++)
  {
    if (*pBuf++ != j++)
    {
      bsp_INFO("Write error j=%d", j);
      break;
    }
  }

  /* 打印速度 */
  bsp_INFO("64MB Write : time:%dms , speed: %dMB/s",
         iTime2 - iTime1, (SDRAM_SIZE / 1024 / 1024 * 1000) / (iTime2 - iTime1));
}

/**
 * @brief 讀SDRAM速度測試(64MB)
 */
void bsp_ReadSpeedTest(void)
{
  uint32_t i;
  uint32_t iTime1, iTime2;
  uint32_t *pBuf;
  static __IO uint32_t ulTemp; /* 設置為__IO類型, 且設置為靜態局部變量，防止被MDK優化 */

  pBuf = (uint32_t *)SDRAM_BANK_ADDR;
  iTime1 = bsp_GetRunTime(); //記錄開始時間
  /* 讀取SDRAM所有空間資料 */
  for (i = 1024 * 1024 / 4; i > 0; i--)
  {
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;

    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;

    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;

    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;

    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;

    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;

    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;

    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
    ulTemp = *pBuf++;
  }

  iTime2 = bsp_GetRunTime(); /* 紀錄結束時間 */
  /* 打印速度 */
  bsp_INFO("64MB Read : time: %dms , speed: %dMB/s",
         iTime2 - iTime1, (SDRAM_SIZE / 1024 / 1024 * 1000) / (iTime2 - iTime1));
}

/*********************************************END OF FILE**********************/
