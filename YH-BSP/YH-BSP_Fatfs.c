#include "YH-BSP_Fatfs.h"
#include "fatfs.h"

RAM_D1 BYTE ReadBuffer[128] = {0};
RAM_D1 BYTE WriteBuffer[128] = {0};

void bsp_Fatfs_SD_Mount(void)
{
    retSD = f_mount(&SDFatFS, SDPath, 1);
    if (retSD == FR_NO_FILESYSTEM)
    {
        bsp_INFO("SDCard is NO_FILESYSTEM, Please format...");
    }
    else if (retSD != FR_OK)
    {
        bsp_INFO("SDCard Mount Error (%d)", retSD);
    }
    else
    {
        bsp_INFO("SDCard Mount Success");
    }
}

void bsp_Fatfs_SD_Unmount(void)
{
    retSD = f_mount(NULL, SDPath, 1);
    if (retSD != FR_OK)
    {
        bsp_INFO("SDCard Mount Error (%d)", retSD);
    }
    else
    {
        bsp_INFO("SDCard Unmount Success");
    }
}
void bsp_Fatfs_SD_Test(void)
{
    char message[] = "STM32H750V_STM32H750_TEST!!";
    memcpy(WriteBuffer, message, strlen(message));
    /*----------------------- 文件系統測試：寫測試 -----------------------------*/
    bsp_INFO("****** 即將進行文件寫入測試... ******");
    retSD = f_open(&SDFile, "0:STM32H750Test.txt", FA_CREATE_ALWAYS | FA_OPEN_APPEND | FA_WRITE);
    if (retSD == FR_OK)
    {
        bsp_INFO("打開/創建STM32H750Test.txt文件成功，向文件寫入數據。");

        retSD = f_write(&SDFile, WriteBuffer, sizeof(WriteBuffer), &fnum);
        if (retSD == FR_OK)
        {
            bsp_INFO("文件寫入成功，寫入字節數據：%d", fnum);
            bsp_INFO("向文件寫入的數據為：\r\n%s", WriteBuffer);
        }
        else
        {
            bsp_INFO("文件寫入失敗：(%d)", retSD);
        }
        f_close(&SDFile);
    }
    else
    {
        bsp_INFO("打開/創建文件失敗");
    }

    /*------------------- 文件系統測試：讀測試 ------------------------------------*/
    bsp_INFO("****** 即將進行文件讀取測試... *****");
    retSD = f_open(&SDFile, "0:STM32H750Test.txt", FA_OPEN_EXISTING | FA_READ);
    if (retSD == FR_OK)
    {
        bsp_INFO("打开文件成功");
        retSD = f_read(&SDFile, ReadBuffer, sizeof(ReadBuffer), &fnum);
        if (retSD == FR_OK)
        {
            bsp_INFO("文件讀取成功,讀到字節數據：%d", fnum);
            bsp_INFO("讀取得的文件數據為：\r\n%s", ReadBuffer);
        }
        else
        {
            bsp_INFO("文件讀取失敗：(%d)", retSD);
        }
    }
    else
    {
        bsp_INFO("打開文件失敗");
    }

    f_close(&SDFile);
}
/*********************************************END OF FILE**********************/
