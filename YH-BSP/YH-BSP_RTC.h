#ifndef __YH_BSP_RTC_H__
#define __YH_BSP_RTC_H__

#include "YH-BSP.h"

#define RTC_BKP_DRX RTC_BKP_DR0 /* 備份暫存器 */
#define RTC_BKP_DATA 0x1314 /* 寫入到備份暫存器的資料 */

void bsp_RTC_AbnormalCheck(void);
void bsp_RTC_TimeAndDate_set(void);
void bsp_RTC_TimeAndDate_Show(void);
void bsp_RTC_Alarm1Set(void);

#endif // __RTC_H__
