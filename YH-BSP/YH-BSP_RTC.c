#include "YH-BSP_RTC.h"

extern RTC_HandleTypeDef hrtc;

static void bsp_RTC_TimeAndDate_init(void);

/**
 * @brief 檢查RTC是否電源異常
 */
void bsp_RTC_AbnormalCheck(void)
{
	/* 檢查是否電源復位 */
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
	{
		bsp_INFO("Power reset...");
	}
	/* 檢查是否外部復位 */
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
	{
		bsp_INFO("External reset...");
	}
	/* 檢查是否軟體復位 */
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)
	{
		bsp_INFO("Software reset...");
	}

	if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DRX) != RTC_BKP_DATA)
	{
		bsp_INFO("RTC TimeAndDate initialization...");
		bsp_RTC_TimeAndDate_init();
	}
}

/**
 * @brief 設定時間(初始化)
 */
static void bsp_RTC_TimeAndDate_init(void)
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	sTime.Hours = 13;
	sTime.Minutes = 10;
	sTime.Seconds = 0;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
	sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
	sDate.Month = RTC_MONTH_AUGUST;
	sDate.Date = 27;
	sDate.Year = 22;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DRX, RTC_BKP_DATA);
}

/**
 * @brief 設定時間
 */
void bsp_RTC_TimeAndDate_set(void)
{
}

/**
 * @brief RTC_TimeAndDate_Show
 */
void bsp_RTC_TimeAndDate_Show(void)
{
}

/**
 * @brief bsp_RTC_Alarm1Set
 */
void bsp_RTC_Alarm1Set(void)
{
}

/**********************************END OF FILE*************************************/
