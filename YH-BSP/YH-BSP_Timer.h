#ifndef __YH_BSP_TIMER_H__
#define __YH_BSP_TIMER_H__
#include "YH-BSP.h"
/* 提供給其他C文件調用的函數 */
extern __IO uint8_t g_ucEnableSystickISR;
int32_t bsp_GetRunTime(void);
int32_t bsp_CheckRunTime(int32_t _LastTime);

void bsp_SysTick_ISR(void);
void bsp_InitHardTimer(void);
void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void *_pCallBack);
#endif

/***************************** (END OF FILE) *********************************/
