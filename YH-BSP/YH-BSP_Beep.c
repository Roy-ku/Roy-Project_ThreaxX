#include "YH-BSP_Beep.h"

/**
 * @brief 初始化蜂鳴器
 */
void BEEP_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  BEEP_GPIO_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BEEP_GPIO_PORT, BEEP_GPIO_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = BEEP_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStruct);
}
/*********************************************END OF FILE**********************/
