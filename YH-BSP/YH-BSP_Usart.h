#ifndef __YH_BSP_USART_H
#define __YH_BSP_USART_H

#include "YH-BSP.h"

#define RXBUFF_SIZE 32

void bsp_USART1_SendString(char *str);
void bsp_USART_IRQHandler(UART_HandleTypeDef *huart);
#endif /* __YH_BSP_USART_H */
