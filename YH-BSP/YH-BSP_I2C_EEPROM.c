#include "YH-BSP_I2C_EEPROM.h"

extern I2C_HandleTypeDef hi2c1;
#define hI2Cx hi2c1
#define EEPROM_PAGESIZE EEROMDrv->PageSize()

#define DATA_Size 32
RAM_D2 uint8_t I2c_Buf_Write[DATA_Size];
RAM_D2 uint8_t I2c_Buf_Read[DATA_Size];

EEPROM_DrvTypeDef *EEROMDrv;

/* 僅供內部使用 */

static bsp_Status bsp_I2C_EEPROM_PageWrite(uint8_t drvaddr, uint8_t *pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite);

/****************************************************************************************************/

/**
 * @brief 將緩衝區中的數據寫到I2C EEPROM中
 * @param pBuffer 緩衝區指針
 * @param WriteAddr 寫地址
 * @param NumByteToWrite 寫的字節數
 * @return bsp_Status 返回說明
 */
bsp_Status bsp_I2C_EEPROM_BufferWrite(uint8_t drvaddr, uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
  bsp_Status status = bsp_PASSED;
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
  Addr = WriteAddr % EEPROM_PAGESIZE;
  count = EEPROM_PAGESIZE - Addr;
  NumOfPage = NumByteToWrite / EEPROM_PAGESIZE;
  NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;

  /* If WriteAddr is I2C_PageSize aligned  */
  if (Addr == 0)
  {
    /* If NumByteToWrite < I2C_PageSize */
    if (NumOfPage == 0)
    {
      status = (bsp_Status)bsp_I2C_EEPROM_PageWrite(drvaddr, pBuffer, WriteAddr, NumOfSingle);
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      while (NumOfPage--)
      {
        status = (bsp_Status)bsp_I2C_EEPROM_PageWrite(drvaddr, pBuffer, WriteAddr, EEPROM_PAGESIZE);
        WriteAddr += EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        status = (bsp_Status)bsp_I2C_EEPROM_PageWrite(drvaddr, pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else
  {
    /* If NumByteToWrite < I2C_PageSize */
    if (NumOfPage == 0)
    {
      status = (bsp_Status)bsp_I2C_EEPROM_PageWrite(drvaddr, pBuffer, WriteAddr, NumOfSingle);
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage = NumByteToWrite / EEPROM_PAGESIZE;
      NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;

      if (count != 0)
      {
        status = (bsp_Status)bsp_I2C_EEPROM_PageWrite(drvaddr, pBuffer, WriteAddr, count);
        WriteAddr += count;
        pBuffer += count;
      }

      while (NumOfPage--)
      {
        status = (bsp_Status)bsp_I2C_EEPROM_PageWrite(drvaddr, pBuffer, WriteAddr, EEPROM_PAGESIZE);
        WriteAddr += EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;
      }
      if (NumOfSingle != 0)
      {
        status = (bsp_Status)bsp_I2C_EEPROM_PageWrite(drvaddr, pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }

  return status;
}

/**
 * @brief 寫一個字節到I2C EEPROM中
 * @param pBuffer 緩衝區指針
 * @param WriteAddr 寫地址
 * @return uint8_t 返回說明
 */
bsp_Status bsp_I2C_EEPROM_ByteWrite(uint8_t drvaddr, uint8_t value, uint8_t WriteAddr)
{
  bsp_Status status = bsp_PASSED;

  status = (bsp_Status)bsp_I2Cx_bus_Write_8bit(&hI2Cx, drvaddr, WriteAddr, value);

  /* Check if the EEPROM is ready for a new operation */
  while (bsp_I2Cx_bus_IsDeviceReady(&hI2Cx, drvaddr, EEPROM_MAX_TRIALS, I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT)
    ;

  return status;
}

/**
 * @brief 在EEPROM的一個寫循環中可以寫多個字節，但一次寫入的字節數
 *        不能超過EEPROM頁的大小，AT24C02每頁有8個字節
 * @param pBuffer 緩衝區指針
 * @param WriteAddr 寫地址
 * @param NumByteToWrite 寫的字節數
 * @return uint8_t  0:OK 1:ERROR
 */
static bsp_Status bsp_I2C_EEPROM_PageWrite(uint8_t drvaddr, uint8_t *pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
  bsp_Status status = bsp_PASSED;
  /* Write EEPROM_PAGESIZE */
  status = (bsp_Status)bsp_I2Cx_bus_WriteMultiple_8bit(&hI2Cx, drvaddr, WriteAddr, pBuffer, NumByteToWrite);

  /* Check if the EEPROM is ready for a new operation */
  while (bsp_I2Cx_bus_IsDeviceReady(&hI2Cx, drvaddr, EEPROM_MAX_TRIALS, I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT)
    ;

  return status;
}

/**
 * @brief 從EEPROM裡面讀取一塊數據
 * @param pBuffer 存放從EEPROM讀取的數據的緩衝區指針
 * @param ReadAddr 接收數據的EEPROM的地址
 * @param NumByteToRead 要從EEPROM讀取的字節數
 * @return uint8_t 0:OK 1:ERROR
 */
bsp_Status bsp_I2C_EEPROM_BufferRead(uint8_t drvaddr, uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
  bsp_Status status = bsp_PASSED;
  status = (bsp_Status)bsp_I2Cx_bus_ReadMultiple_8bit(&hI2Cx, drvaddr, ReadAddr, pBuffer, NumByteToRead);
  return status;
}

/**
 * @brief I2C(AT24C02)讀寫測試
 * @return uint8_t 0:OK 1:ERROR
 */
bsp_Status bsp_I2C_EEPROM_Test(void)
{

  uint16_t i;

  EEROMDrv = &at24c02_drv;
  AppPrintf("AT24C02 Addr : 0x%x\r\n", EEROMDrv->DrvAddr());

  /*填充資料*/
  for (i = 0; i < DATA_Size; i++)
  {
    I2c_Buf_Write[i] = DATA_Size - i;
  }

  EEROMDrv->BufferWrite(I2c_Buf_Write, 0x00, DATA_Size);
  EEROMDrv->BufferRead(I2c_Buf_Read, 0x00, DATA_Size);

  /* 校驗資料 */
  for (i = 0; i < DATA_Size; i++)
  {
    if (I2c_Buf_Read[i] != I2c_Buf_Write[i])
    {
      AppPrintf("0x%02X ", I2c_Buf_Read[i]);
      AppPrintf("I2C(AT24C02)test error");
      return bsp_FAILED;
    }
    AppPrintf("0x%02X ", I2c_Buf_Read[i]);
    if (i % 8 == 7)
    {
      AppPrintf("\r\n");
    }
  }
  AppPrintf("\r\nI2C(AT24C02)test ok~\r\n");
  return bsp_PASSED;
}

/*********************************************END OF FILE**********************/
