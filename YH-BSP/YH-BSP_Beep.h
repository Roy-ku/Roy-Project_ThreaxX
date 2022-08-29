#ifndef __YH_BSP_BEEP_H
#define __YH_BSP_BEEP_H

#include "YH-BSP.h"

/* 定義蜂鳴器連接的GPIO端口 */
#define BEEP_GPIO_PORT BEEP_GPIO_Port                       
#define BEEP_GPIO_CLK_ENABLE() __HAL_RCC_GPIOI_CLK_ENABLE() 
#define BEEP_GPIO_PIN BEEP_Pin                            

/* 定義控制IO的宏 */
// #define BEEP_TOGGLE digitalToggle(BEEP_GPIO_PORT, BEEP_GPIO_PIN)
// #define BEEP_ON digitalHi(BEEP_GPIO_PORT, BEEP_GPIO_PIN)
// #define BEEP_OFF digitalLo(BEEP_GPIO_PORT, BEEP_GPIO_PIN)
#define BEEP_ON HAL_GPIO_WritePin(BEEP_GPIO_PORT, BEEP_GPIO_PIN,GPIO_PIN_SET)
#define BEEP_OFF HAL_GPIO_WritePin(BEEP_GPIO_PORT, BEEP_GPIO_PIN,GPIO_PIN_RESET)
#define BEEP_TOGGLE HAL_GPIO_TogglePin(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
void BEEP_GPIO_Config(void);

#endif /* __YH_BSP_BEEP_H */
