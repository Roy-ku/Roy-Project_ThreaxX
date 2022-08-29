#ifndef __RTOS_THREAD_H
#define __RTOS_THREAD_H
#include "YH-BSP.h"

extern TX_MUTEX AppI2CSemp;
void AppPrintf(const char *fmt, ...);
UINT AppTaskCreate(VOID *memory_ptr);
#endif //__RTOS_THREAD_H
/***************************** (END OF FILE) *********************************/
