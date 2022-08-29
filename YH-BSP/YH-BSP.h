#ifndef __YH_BSP_H
#define __YH_BSP_H

#include "main.h"
#include "dma.h"
#include "dma2d.h"
//#include "fatfs.h"
#include "i2c.h"
#include "ltdc.h"
#include "quadspi.h"
//#include "sdmmc.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"


#include "stdio.h"
#include "string.h"

/* BSP */
#include "YH-BSP_def.h" // 順序須為第一個
#include "YH-BSP_Usart.h"
#include "YH-BSP_LCD.h"
#include "YH-BSP_LED.h"
#include "YH-BSP_Timer.h"
#include "YH-BSP_Button_FIFO.h"
#include "YH-BSP_Beep.h"
#include "YH-BSP_FMC_sdram.h"
#include "YH-BSP_I2Cx_bus.h"
#include "YH-BSP_I2C_EEPROM.h"
#include "YH-BSP_GTxx_Touch.h"
#include "YH-BSP_RTC.h"
//#include "YH-BSP_Fatfs.h"
//#include "YH-BSP_qspiflash.h"

/* Components */
#include "AT24C02.h"
#include "GT9xx.h"

/* RTOS */
#define BSP_USE_OS
//#define bsp_OS_NOS
//#define bsp_OS_RTX
#define bsp_OS_THREADX
#include "app_threadx.h"
#include "rtos_thread.h"

#if !defined(BSP_USE_OS) && defined(bsp_OS_NOS)
	#define bsp_Delay(millisecond) HAL_Delay(millisecond)
#endif // bsp_OS_NOS
#if defined(BSP_USE_OS) && defined(bsp_OS_RTX)
	#define bsp_Delay(millisecond) osDelay(millisecond)
#endif // bsp_OS_NOS
#if defined(BSP_USE_OS) && defined(bsp_OS_THREADX)
	#define bsp_Delay(millisecond) tx_thread_sleep(millisecond)
#endif // BSP_USE_OS

#define Enable_EventRecorder 0
#if Enable_EventRecorder == 1
#include "EventRecorder.h"
#endif



void bsp_Init(void);
bsp_Status bsp_Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint32_t BufferLength);
void bsp_WIFI_PDN_INIT(void);
void bsp_RunPer1ms(void);
void bsp_RunPer5ms(void);
void bsp_RunPer10ms(void);
void bsp_RunPer20ms(void);
void bsp_Key1_DOWN(void);
void bsp_Key1_UP(void);
void bsp_Key1_LONG(void);
void bsp_Key2_DOWN(void);
void bsp_Key2_UP(void);
void bsp_Key2_LONG(void);

#endif /* __YH_BSP_H */
