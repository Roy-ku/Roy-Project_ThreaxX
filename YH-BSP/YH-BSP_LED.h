#ifndef __YH_BSP_LED_H
#define __YH_BSP_LED_H
#include "YH-BSP.h"

/* 定義控制IO的宏 */
#define LED_R_TOGGLE HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin)
#define LED_R_OFF HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET)
#define LED_R_ON HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET)

#define LED_G_TOGGLE HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin)
#define LED_G_OFF HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET)
#define LED_G_ON HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET)

#define LED_B_TOGGLE HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin)
#define LED_B_OFF HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET)
#define LED_B_ON HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET)

/* 基本混色*/
//红
#define LED_RED \
	LED_R_ON;   \
	LED_G_OFF;  \
	LED_B_OFF;

//綠
#define LED_GREEN \
	LED_R_OFF;    \
	LED_G_ON;     \
	LED_B_OFF;

//藍
#define LED_BLUE \
	LED_R_OFF;   \
	LED_G_OFF;   \
	LED_B_ON;

//黄(红+綠)
#define LED_YELLOW \
	LED_R_ON;      \
	LED_G_ON;      \
	LED_B_OFF;

//紫(红+藍)
#define LED_PURPLE \
	LED_R_ON;      \
	LED_G_OFF      \
	LED_B_ON

//青(綠+藍)
#define LED_CYAN \
	LED_R_OFF;   \
	LED_G_ON;    \
	LED_B_ON;

//白(红+綠+藍)
#define LED_WHITE \
	LED_R_ON;     \
	LED_G_ON;     \
	LED_B_ON;

//(全部關閉)
#define LED_RGBOFF \
	LED_R_OFF;     \
	LED_G_OFF      \
	LED_B_OFF

typedef enum
{
	LED_GPIO_MOD = 0,
	LED_PWM_MOD,
} LEDRGB_MOD_E;

typedef struct _LEDRGB_T
{
	uint8_t mod;
	GPIO_TypeDef *gpioport;
	uint16_t gpiopin;
	TIM_HandleTypeDef *htim;
	TIM_TypeDef *tim;
	uint16_t timchannel;
} LEDRGB_T;
extern LEDRGB_T g_tLEDR;
extern LEDRGB_T g_tLEDG;
extern LEDRGB_T g_tLEDB;
void bsp_LED_GPIO_Init(void);
void bsp_LED_PWM_Start(void);
void bsp_LED_PWM_Stop(void);
void bsp_LED_R_PWM(void);
void bsp_LED_G_PWM(void);
void bsp_LED_B_PWM(void);
#endif /* __YH_BSP_LED_H */
