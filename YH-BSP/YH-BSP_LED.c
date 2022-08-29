#include "YH-BSP_LED.h"

/* RGBLED 宏定義*/
#define LED_R_GPIO_CLK_ENABLE __GPIOB_CLK_ENABLE()
#define LED_R_GPIO_CLK_DISABLE __GPIOB_CLK_DISABLE()

#define LED_G_GPIO_CLK_ENABLE __GPIOB_CLK_ENABLE()
#define LED_G_GPIO_CLK_DISABLE __GPIOB_CLK_DISABLE()

#define LED_B_GPIO_CLK_ENABLE __GPIOA_CLK_ENABLE()
#define LED_B_GPIO_CLK_DISABLE __GPIOA_CLK_DISABLE()

#define LED_R_TIM_CLK_ENABLE __TIM3_CLK_ENABLE()
#define LED_R_TIM_CLK_DISABLE __TIM3_CLK_DISABLE()

#define LED_G_TIM_CLK_ENABLE __TIM3_CLK_ENABLE()
#define LED_G_TIM_CLK_DISABLE __TIM3_CLK_DISABLE()

#define LED_B_TIM_CLK_ENABLE __TIM2_CLK_ENABLE()
#define LED_B_TIM_CLK_DISABLE __TIM2_CLK_DISABLE()

LEDRGB_T g_tLEDR = {
	.mod = LED_PWM_MOD,
	.gpioport = LED_R_GPIO_Port,
	.gpiopin = LED_R_Pin,
	.htim = &htim3,
	.tim = TIM3,
	.timchannel = TIM_CHANNEL_3,
};

LEDRGB_T g_tLEDG = {
	.mod = LED_PWM_MOD,
	.gpioport = LED_G_GPIO_Port,
	.gpiopin = LED_G_Pin,
	.htim = &htim3,
	.tim = TIM3,
	.timchannel = TIM_CHANNEL_4,
};

LEDRGB_T g_tLEDB = {
	.mod = LED_PWM_MOD,
	.gpioport = LED_B_GPIO_Port,
	.gpiopin = LED_B_Pin,
	.htim = &htim2,
	.tim = TIM2,
	.timchannel = TIM_CHANNEL_4,
};

/*****************************************************************/
void bsp_LEDRGB_GPIO_CLK_Enable()
{
	LED_R_GPIO_CLK_ENABLE;
	LED_G_GPIO_CLK_ENABLE;
	LED_B_GPIO_CLK_ENABLE;
}

void bsp_LEDRGB_GPIO_CLK_DISABLE()
{
	LED_R_GPIO_CLK_DISABLE;
	LED_G_GPIO_CLK_DISABLE;
	LED_B_GPIO_CLK_DISABLE;
}

void bsp_LEDRGB_TIM_CLK_Enable()
{
	LED_R_TIM_CLK_ENABLE;
	LED_G_TIM_CLK_ENABLE;
	LED_B_TIM_CLK_ENABLE;
}

void bsp_LEDRGB_TIM_CLK_DISABLE()
{
	LED_R_TIM_CLK_DISABLE;
	LED_G_TIM_CLK_DISABLE;
	LED_B_TIM_CLK_DISABLE;
}

/*****************************************************************/

static uint8_t bsp_GetAFofTIM(TIM_TypeDef *led_rgb_tim)
{
	uint8_t ret = 0;
	if (led_rgb_tim == TIM2)
	{
		ret = GPIO_AF1_TIM2;
	}

	else if (led_rgb_tim == TIM3)
	{
		ret = GPIO_AF2_TIM3;
	}

	return ret;
}

static void bsp_ConfigGpioOut(LEDRGB_T led_rgb_t, GPIO_PinState led_rgb_state)
{
	if (led_rgb_t.mod != LED_GPIO_MOD)
	{
		led_rgb_t.mod = LED_GPIO_MOD;
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		bsp_LEDRGB_GPIO_CLK_Enable();
		GPIO_InitStruct.Pin = led_rgb_t.gpiopin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(led_rgb_t.gpioport, &GPIO_InitStruct);
	}
	HAL_GPIO_WritePin(led_rgb_t.gpioport, led_rgb_t.gpiopin, led_rgb_state);
}

static void bsp_ConfigTimGpio(GPIO_TypeDef *led_rgb_port, uint16_t led_rgb_pin, TIM_TypeDef *led_rgb_tim)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	bsp_LEDRGB_TIM_CLK_Enable();
	GPIO_InitStruct.Pin = led_rgb_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = bsp_GetAFofTIM(led_rgb_tim);
	HAL_GPIO_Init(led_rgb_port, &GPIO_InitStruct);
}

/**
 * @brief  初始化控制LED的IO
 * @param  无
 * @retval 无
 */
void bsp_LED_GPIO_Init(void)
{
	bsp_ConfigGpioOut(g_tLEDR, GPIO_PIN_SET);
	bsp_ConfigGpioOut(g_tLEDG, GPIO_PIN_SET);
	bsp_ConfigGpioOut(g_tLEDB, GPIO_PIN_SET);
}

void bsp_LED_PWM_Start(void)
{
	bsp_LEDRGB_GPIO_CLK_Enable();
	bsp_LEDRGB_TIM_CLK_Enable();
	HAL_TIM_PWM_Start(g_tLEDB.htim, g_tLEDB.timchannel);
	HAL_TIM_PWM_Start(g_tLEDR.htim, g_tLEDR.timchannel);
	HAL_TIM_PWM_Start(g_tLEDG.htim, g_tLEDG.timchannel);
	__HAL_TIM_SET_COMPARE(g_tLEDB.htim, g_tLEDB.timchannel, 0);
	__HAL_TIM_SET_COMPARE(g_tLEDR.htim, g_tLEDR.timchannel, 0);
	__HAL_TIM_SET_COMPARE(g_tLEDG.htim, g_tLEDG.timchannel, 0);
}

void bsp_LED_PWM_Stop(void)
{
	__HAL_TIM_SET_COMPARE(g_tLEDB.htim, g_tLEDB.timchannel, 0);
	__HAL_TIM_SET_COMPARE(g_tLEDR.htim, g_tLEDR.timchannel, 0);
	__HAL_TIM_SET_COMPARE(g_tLEDG.htim, g_tLEDG.timchannel, 0);
	bsp_LEDRGB_TIM_CLK_DISABLE();
	HAL_TIM_PWM_Stop(g_tLEDB.htim, g_tLEDB.timchannel);
	HAL_TIM_PWM_Stop(g_tLEDR.htim, g_tLEDR.timchannel);
	HAL_TIM_PWM_Stop(g_tLEDG.htim, g_tLEDG.timchannel);
}

void bsp_LED_R_PWM(void)
{
	static int16_t x = 0;
	static int8_t y = 100;
	if (x >= 10000 - 1)
	{
		y = -100;
	}
	if (x <= 0)
	{
		y = 100;
	}
	__HAL_TIM_SET_COMPARE(g_tLEDR.htim, g_tLEDR.timchannel, x);
	x = x + y;
}

void bsp_LED_G_PWM(void)
{
	static int16_t x = 0;
	static int8_t y = 100;
	if (x >= 10000 - 1)
	{
		y = -100;
	}
	if (x <= 0)
	{
		y = 100;
	}
	__HAL_TIM_SET_COMPARE(g_tLEDG.htim, g_tLEDG.timchannel, x);
	x = x + y;
}

void bsp_LED_B_PWM(void)
{
	static int16_t x = 0;
	static int8_t y = 100;
	if (x >= 10000 - 1)
	{
		y = -100;
	}
	if (x <= 0)
	{
		y = 100;
	}
	__HAL_TIM_SET_COMPARE(g_tLEDB.htim, g_tLEDB.timchannel, x);
	x = x + y;
}

/*********************************************END OF FILE**********************/
