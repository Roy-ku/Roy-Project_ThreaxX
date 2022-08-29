#include "YH-BSP.h"

uint8_t LCD_Test = 1;

void bsp_Init(void)
{

#if (Enable_EventRecorder == 1)
	EventRecorderInitialize(EventRecordAll, 1U);
#endif

/* AXI SRAM的時鐘是上電自動使能的，而D2域的SRAM1，SRAM2和SRAM3要單獨使能 */
#if (Enable_D2SRAM == 1)
	__HAL_RCC_D2SRAM1_CLK_ENABLE();
	__HAL_RCC_D2SRAM2_CLK_ENABLE();
	__HAL_RCC_D2SRAM3_CLK_ENABLE();
#endif
	bsp_INFO();
	bsp_INFO("SystemCoreClock: %d", SystemCoreClock);
	bsp_INFO("HCLKFreq: %d", HAL_RCC_GetHCLKFreq());

	bsp_RTC_AbnormalCheck();
	bsp_WIFI_PDN_INIT();
	Initial_Key_Input_FIFO();
	// bsp_LED_GPIO_Init();
	bsp_LED_PWM_Start();
	GTP_Init_Panel();
}
/*******************************************************************************************************/
void bsp_WIFI_PDN_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
}

/**
 * @brief 比較Buffer
 * @param pBuffer1 Buffer1
 * @param pBuffer2 Buffer
 * @param BufferLength Buffer Size
 * @return bsp_Status PASSED : 結果一致 , FAILED : 結果不一致
 */
bsp_Status bsp_Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint32_t BufferLength)
{
	while (BufferLength--)
	{
		if (*pBuffer1 != *pBuffer2)
		{
			return bsp_FAILED;
		}

		pBuffer1++;
		pBuffer2++;
	}
	return bsp_PASSED;
}

/*******************************************************************************************************/

/**
 * @brief 該函數每隔1ms被Systick中斷調用1次。一些需要周期性處理的事務
 *			 可以放在此函數。比如：觸摸坐標掃描。
 */
void bsp_RunPer1ms(void)
{
}

/**
 * @brief 該函數每隔5ms被Systick中斷調用1次。一些處理時間要求不嚴格的
 *			任務可以放在此函數。比如：按鍵掃描、蜂鳴器鳴叫控制等。
 */
void bsp_RunPer5ms(void)
{
}

/**
 * @brief 該函數每隔10ms被Systick中斷調用1次。一些處理時間要求不嚴格的
 *			任務可以放在此函數。比如：按鍵掃描、蜂鳴器鳴叫控制等。
 */
void bsp_RunPer10ms(void)
{
	bsp_KeyScan10ms();
	bsp_LED_R_PWM();
	//  bsp_LED_G_PWM();
	//  bsp_LED_B_PWM();
}
/**
 * @brief 該函數每隔20ms被Systick中斷調用1次。一些處理時間要求不嚴格的
 *			任務可以放在此函數。比如：按鍵掃描、蜂鳴器鳴叫控制等。
 */
void bsp_RunPer20ms(void)
{
	GTP_Execu(&Touch_X, &Touch_Y, &Touch_W);
}
/*******************************************************************************************************/
/**
 * @brief Key1按下
 */
void bsp_Key1_DOWN(void)
{
	// bsp_INFO("K1 DOWN\r\n");
	//  if (LCD_Test > 1 && LCD_Test != 1)
	//  {
	//  	LCD_Test--;
	//  }

	//__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
}

/**
 * @brief Key1彈起
 */
void bsp_Key1_UP(void)
{
	// bsp_INFO("K1 UP\r\n");
}

/**
 * @brief Key1長按
 */
void bsp_Key1_LONG(void)
{
	// bsp_INFO("K1 LONG\r\n");
}

/**
 * @brief Key2按下
 */
void bsp_Key2_DOWN(void)
{
	// bsp_INFO("K2 DOWN\r\n");
	if (LCD_Test < 5 && LCD_Test != 5)
	{
		LCD_Test++;
	}
	if (LCD_Test == 5)
	{
		LCD_Test = 0;
	}
	//__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 10000 - 1);
}

/**
 * @brief Key2彈起
 */
void bsp_Key2_UP(void)
{
	// bsp_INFO("K2 UP\r\n");
}

/**
 * @brief Key2長按
 */
void bsp_Key2_LONG(void)
{
	// bsp_INFO("K2 LONG\r\n");
}
/*********************************************END OF FILE**********************/
