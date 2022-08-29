#include "YH-BSP_Usart.h"

RAM_D2 uint8_t UART_RXBUFF[RXBUFF_SIZE] = {0};

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart1)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		HAL_UART_DMAStop(&huart1);
		SCB_CleanInvalidateDCache();
		uint8_t mgs1[] = "RXBUFF OVER!\r\n";
		HAL_UART_Transmit(&huart1, mgs1, sizeof(mgs1) - 1, 0x50);
		memset(UART_RXBUFF, '\0', RXBUFF_SIZE);
		HAL_UART_Receive_DMA(&huart1, UART_RXBUFF, RXBUFF_SIZE);
	}
}

void bsp_USART_IRQHandler(UART_HandleTypeDef *huart)
{
	if (huart == &huart1)
	{
		if (RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_ORE))
		{
			__HAL_UART_CLEAR_OREFLAG(&huart1);
			__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		}

		if (RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))
		{
			__HAL_UART_CLEAR_IDLEFLAG(&huart1);
			HAL_UART_DMAStop(&huart1);
			SCB_CleanInvalidateDCache();
			// SCB_InvalidateDCache_by_Addr((uint32_t *)UART_RXBUFF, 1);
			uint8_t data_length = RXBUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
			HAL_UART_Transmit(&huart1, UART_RXBUFF, data_length, 0x50);
			memset(UART_RXBUFF, '\0', RXBUFF_SIZE);
			HAL_UART_Receive_DMA(&huart1, UART_RXBUFF, RXBUFF_SIZE);
		}
	}
}

void bsp_USART1_SendString(char *str)
{
	unsigned int count = 0;
	do
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)(str + count), 1, 1000);
		count++;
	} while (*(str + count) != '\0');
}

#if (Enable_EventRecorder != 1)
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);
	return (ch);
}

int fgetc(FILE *f)
{
	int ch;
	HAL_UART_Receive(&huart1, (uint8_t *)&ch, 1, 1000);
	return (ch);
}
#endif
/*********************************************END OF FILE**********************/
