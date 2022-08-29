#ifndef __YH_BSP_QSPI_DUALFLASH_H
#define __YH_BSP_QSPI_DUALFLASH_H

#include "YH-BSP.h"

/* Private typedef -----------------------------------------------------------*/
#define sFLASH_ID 0XEF4019 // W25Q256JVEM

/* QSPI Error codes */
#define QSPI_OK ((uint8_t)0x00)
#define QSPI_ERROR ((uint8_t)0x01)

/* QSPI Info */
typedef struct
{
  uint32_t FlashSize;          /* 閃存大小 */
  uint32_t EraseSectorSize;    /* 擦除操作的扇區大小 */
  uint32_t EraseSectorsNumber; /* 擦除操作的扇區數 */
  uint32_t ProgPageSize;       /* 編程操作的頁面大小 */
  uint32_t ProgPagesNumber;    /* 編程操作的頁面數 */
} QSPI_Info;

/**
 * @brief  W25Q256JV(256 MBits)  FLASH
 */
#define DUALFLASH 1

#if (DUALFLASH == 0)
#define W25Q256JV_FLASH_SIZE 0x2000000 /* 256 MBits => 32MBytes */
#elif (DUALFLASH == 1)
#define W25Q256JV_FLASH_SIZE 0x4000000 /* 256 * 2 MBits => 64MBytes */
#endif



#define W25Q256JV_BLOCK_SIZE 0x10000 /* 64KB */
#define W25Q256JV_SECTOR_SIZE 0x1000 /* 4K */
#define W25Q256JV_PAGE_SIZE 0x100    /*  256 bytes */

#define W25Q256JV_DUMMY_CYCLES_READ 4
#define W25Q256JV_DUMMY_CYCLES_READ_QUAD 10

#define W25Q256JV_BULK_ERASE_MAX_TIME 250000
#define W25Q256JV_SECTOR_ERASE_MAX_TIME 3000
#define W25Q256JV_SUBSECTOR_ERASE_MAX_TIME 800

/**
 * @brief  W25Q256JV 指令
 */

/* 復位操作 */
#define RESET_ENABLE_CMD 0x66
#define RESET_MEMORY_CMD 0x99

#define ENTER_QPI_MODE_CMD 0x38
#define EXIT_QPI_MODE_CMD 0xFF

/* 識別操作 */
#define READ_ID_CMD 0x90
#define DUAL_READ_ID_CMD 0x92
#define QUAD_READ_ID_CMD 0x94
#define READ_JEDEC_ID_CMD 0x9F

/* 讀操作 */
#define READ_CMD_4BYTE 0x03 // 使用該命令時 Clock MAX 50MHz
#define FAST_READ_CMD 0x0B
#define DUAL_OUT_FAST_READ_CMD 0x3B
#define DUAL_INOUT_FAST_READ_CMD 0xBB
#define QUAD_OUT_FAST_READ_CMD 0x6B
#define QUAD_INOUT_FAST_READ_CMD 0xEB
#define QUAD_INOUT_FAST_READ_CMD_4BYTE 0xEC

/* 寫使能操作 */
#define WRITE_ENABLE_CMD 0x06
#define WRITE_DISABLE_CMD 0x04

/* 暫存器操作 */
#define READ_STATUS_REG1_CMD 0x05
#define READ_STATUS_REG2_CMD 0x35
#define READ_STATUS_REG3_CMD 0x15

#define WRITE_STATUS_REG1_CMD 0x01
#define WRITE_STATUS_REG2_CMD 0x31
#define WRITE_STATUS_REG3_CMD 0x11

/* 编程操作 */
#define PAGE_PROG_CMD 0x02
#define QUAD_INPUT_PAGE_PROG_CMD_4BYTE 0x34
#define EXT_QUAD_IN_FAST_PROG_CMD_4BYTE 0x12
#define ENTER_4_BYTE_ADDR_MODE_CMD 0xB7
#define EXIT_4_BYTE_ADDR_MODE_CMD 0xE9

/* 擦除操作 */
#define SECTOR_ERASE_CMD 0x20        // 4K 24bit
#define SECTOR_ERASE_CMD_4BYTE 0x21  // 4K 32bit
#define BLOCK32_ERASE_CMD 0x52       // 32K 24bit
#define BLOCK64_ERASE_CMD 0xD8       // 64K 24bit
#define BLOCK64_ERASE_CMD_4BYTE 0xDC // 64K 32bit
#define CHIP_ERASE_CMD 0xC7

//#define PROG_ERASE_RESUME_CMD                 0x7A
//#define PROG_ERASE_SUSPEND_CMD                0x75

/* 狀態暫存器標誌 */
#define W25Q256JV_FSR_BUSY ((uint16_t)0x0101) /*!< busy */
#define W25Q256JV_FSR_WREN ((uint16_t)0x0202) /*!< write enable */
#define W25Q256JV_FSR_QE ((uint8_t)0x06)      /* quad enable */

/*命令定义-结尾*******************************/

uint8_t bsp_QSPI_Flash_GetInfo(QSPI_Info *pInfo);
uint8_t bsp_QSPI_Flash_Init(void);
uint8_t bsp_QSPI_Flash_Erase_Sector(uint32_t SectorAddress);
uint8_t bsp_QSPI_Flash_Erase_Block32(uint32_t BlockAddress);
uint8_t bsp_QSPI_Flash_Erase_Block64(uint32_t BlockAddress);
uint8_t bsp_QSPI_Flash_Erase_Chip(void);
uint8_t bsp_QSPI_Flash_Read(uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
uint8_t bsp_QSPI_Flash_FastRead(uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
uint8_t bsp_QSPI_Flash_Write(uint8_t *pData, uint32_t WriteAddr, uint32_t Size);
uint32_t bsp_QSPI_Flash_ReadDeviceID(void);
uint32_t bsp_QSPI_Flash_ReadID(void);
uint8_t bsp_QSPI_Flash_EnableMemoryMappedMode(void);
uint8_t bsp_QSPI_Flash_QuitMemoryMappedMode(void);
uint8_t bsp_QSPI_Flash_ResetMemory(void);
bsp_Status bsp_QSPI_Flash_Test(void);
#endif /* __YH_BSP_QSPI_DUALFLASH_H */
