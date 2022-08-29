#ifndef __YH_BSP_DEF_H
#define __YH_BSP_DEF_H

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define ENABLE_INT() NVIC_RESETPRIMASK()
#define DISABLE_INT() NVIC_SETPRIMASK()
#define NVIC_SETPRIMASK() __set_PRIMASK(1)   //禁止全局中斷
#define NVIC_RESETPRIMASK() __set_PRIMASK(0) //使能全局中斷

#define Enable_D2SRAM 1                                            // 使能D2域 SRAM1,SRAM2,SRAM3
#define RAM_DTCM __attribute__((section(".RAM_DTCM"), aligned(4))) // 用於RTOS
#define RAM_D1 __attribute__((section(".RAM_D1"), aligned(32)))    // 用於DMA Buffer SDMMC1,DMA
#define RAM_D2 __attribute__((section(".RAM_D2")))                 // 用於DMA Buffer
#define RAM_D3 __attribute__((section(".RAM_D3")))
#define BackupSRAM __attribute__((section(".BackupSRAM"), aligned(4))) // 用於系統進入低功耗模式後，繼續保存資料（Vbat引腳外接電池）
#define RAM_SDRAM __attribute__((section(".bss.RAM_SDRAM"), aligned(32)))

/* bsp 全局變數 */
extern uint8_t LCD_Test;              // LCD 測試變數
extern int Touch_X, Touch_Y, Touch_W; // 保存觸摸座標訊息
extern uint8_t Touch_event;           // 觸摸事件

/* 信息輸出 */
#define bsp_DEBUG_ON 0

#define bsp_INFO(fmt, arg...) printf("<<-bsp-INFO->>\r\n" fmt "\r\n", ##arg)
#define bsp_ERROR(fmt, arg...) printf("<<-bsp-ERROR INFO->>\r\n" fmt "\r\n", ##arg)
#define bsp_DEBUG(fmt, arg...)                                             \
    do                                                                     \
    {                                                                      \
        if (bsp_DEBUG_ON)                                                  \
            printf("<<-bsp-DEBUG->>\r\n[%d]" fmt "\r\n", __LINE__, ##arg); \
    } while (0)

typedef enum
{
    bsp_PASSED = 0x00,
    bsp_FAILED = 0x01
} bsp_Status;

#endif /* __YH_BSP_DEF_H */
