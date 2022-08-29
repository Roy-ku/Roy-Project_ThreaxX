#ifndef __SDRAM_H
#define __SDRAM_H

#include "YH-BSP.h"

#define SDRAM_BANK_ADDR ((uint32_t)0xD0000000) //SDRAM 首地址
#define SDRAM_SIZE 0x4000000                   //64MByte,兩片W9825G6KH-6,32MB@16bit组成64M@32bit

#define SDRAM_TIMEOUT ((uint32_t)0xFFFF)

/*FMC SDRAM 模式配置的暫存器相關定義*/
#define SDRAM_MODEREG_BURST_LENGTH_1 ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2 ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4 ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8 ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2 ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3 ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE ((uint16_t)0x0200)

/*信息输出*/
#define SDRAM_DEBUG_ON 1

#define SDRAM_INFO(fmt, arg...) printf("<<-SDRAM-INFO->> " fmt "\n", ##arg)
#define SDRAM_ERROR(fmt, arg...) printf("<<-SDRAM-ERROR->> " fmt "\n", ##arg)
#define SDRAM_DEBUG(fmt, arg...)                                  \
  do                                                              \
  {                                                               \
    if (SDRAM_DEBUG_ON)                                           \
      printf("<<-SDRAM-DEBUG->> [%d]" fmt "\n", __LINE__, ##arg); \
  } while (0)

void bsp_SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command);
void bsp_SDRAM_WriteBuffer(uint32_t *pBuffer, uint32_t uwWriteAddress, uint32_t uwBufferSize);
void bsp_SDRAM_ReadBuffer(uint32_t *pBuffer, uint32_t uwReadAddress, uint32_t uwBufferSize);
uint8_t bsp_SDRAM_Test(void);
uint32_t bsp_SDRAM_Test2(void);
void bsp_WriteSpeedTest(void);
void bsp_ReadSpeedTest(void);
#endif /* __SDRAM_H */
