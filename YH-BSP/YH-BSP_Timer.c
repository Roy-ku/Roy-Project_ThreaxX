#include "YH-BSP_Timer.h"

/*
	全局運行時間，單位1ms
	最長可以表示 49天，如果你的產品連續運行時間超過這個數，則必須考慮溢出問題
*/
__IO uint32_t g_iRunTime = 0;

__IO uint8_t g_ucEnableSystickISR = 1; /* 等待變量初始化 */

/*
*********************************************************************************************************
*	函 數 名: bsp_SysTick_ISR
*	功能說明: 放在SysTick中斷服務程序，約每隔1ms進入1次
*	形    參:  無
*	返 回 值: 無
*********************************************************************************************************
*/
void bsp_SysTick_ISR(void)
{
	static uint8_t s_count20ms = 0;
	static uint8_t s_count10ms = 0;
	static uint8_t s_count5ms = 0;
	g_iRunTime++; //全局運行時間每1ms增1

	/* 變量是 uint32_t 類型，最大數為 0xFFFFFFFF */
	if (g_iRunTime == UINT32_MAX)
	{
		g_iRunTime = 0;
	}
	bsp_RunPer1ms(); // 約每隔1ms調用一次此函數

	if (++s_count5ms >= 5)
	{
		s_count5ms = 0;
		bsp_RunPer5ms(); // 約每隔5ms調用一次此函數
	}

	if (++s_count10ms >= 10)
	{
		s_count10ms = 0;
		bsp_RunPer10ms(); // 約每隔10ms調用一次此函數
	}
	if (++s_count20ms >= 20)
	{
		s_count20ms = 0;
		bsp_RunPer20ms(); // 約每隔10ms調用一次此函數
	}
}

/*
*********************************************************************************************************
*	函 數 名: bsp_GetRunTime
*	功能說明: 獲取CPU運行時間，單位1ms。最長可以表示 49天，如果你的產品連續運行時間超過這個數，則必須考慮溢出問題
*	形    參:  無
*	返 回 值: CPU運行時間，單位1ms
*********************************************************************************************************
*/
int32_t bsp_GetRunTime(void)
{
	int32_t runtime;

	DISABLE_INT(); /* 關中斷 */

	runtime = g_iRunTime; /* 這個變量在Systick中斷中被改寫，因此需要關中斷進行保護 */

	ENABLE_INT(); /* 開中斷 */

	return runtime;
}

/*
*********************************************************************************************************
*	函 數 名: bsp_CheckRunTime
*	功能說明: 計算當前運行時間和給定時刻之間的差值。處理了計數器循環。
*	形    參:  _LastTime 上個時刻
*	返 回 值: 當前時間和過去時間的差值，單位1ms
*********************************************************************************************************
*/
int32_t bsp_CheckRunTime(int32_t _LastTime)
{
	int32_t now_time;
	int32_t time_diff;

	DISABLE_INT(); /* 關中斷 */

	now_time = g_iRunTime; /* 這個變量在Systick中斷中被改寫，因此需要關中斷進行保護 */

	ENABLE_INT(); /* 開中斷 */

	if (now_time >= _LastTime)
	{
		time_diff = now_time - _LastTime;
	}
	else
	{
		time_diff = UINT32_MAX - _LastTime + now_time;
	}

	return time_diff;
}

/***************************** (END OF FILE) *********************************/
