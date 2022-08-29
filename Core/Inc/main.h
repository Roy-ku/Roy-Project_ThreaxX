/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "YH-BSP.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define FMC_D28_Pin GPIO_PIN_6
#define FMC_D28_GPIO_Port GPIOI
#define FMC_UDQM2_Pin GPIO_PIN_5
#define FMC_UDQM2_GPIO_Port GPIOI
#define FMC_LDQM2_Pin GPIO_PIN_4
#define FMC_LDQM2_GPIO_Port GPIOI
#define LTDC_B6_Pin GPIO_PIN_5
#define LTDC_B6_GPIO_Port GPIOK
#define QSPI_FLASH_BK2_IO2_Pin GPIO_PIN_9
#define QSPI_FLASH_BK2_IO2_GPIO_Port GPIOG
#define SDMMC1_D2_Pin GPIO_PIN_10
#define SDMMC1_D2_GPIO_Port GPIOC
#define SPI3_NSS_Pin GPIO_PIN_15
#define SPI3_NSS_GPIO_Port GPIOA
#define FMC_D25_Pin GPIO_PIN_1
#define FMC_D25_GPIO_Port GPIOI
#define FMC_D24_Pin GPIO_PIN_0
#define FMC_D24_GPIO_Port GPIOI
#define FMC_D29_Pin GPIO_PIN_7
#define FMC_D29_GPIO_Port GPIOI
#define FMC_UDQM_Pin GPIO_PIN_1
#define FMC_UDQM_GPIO_Port GPIOE
#define SPI3_MISO_Pin GPIO_PIN_4
#define SPI3_MISO_GPIO_Port GPIOB
#define LTDC_B5_Pin GPIO_PIN_4
#define LTDC_B5_GPIO_Port GPIOK
#define LTDC_B3_Pin GPIO_PIN_15
#define LTDC_B3_GPIO_Port GPIOJ
#define SPI3_MOSI_Pin GPIO_PIN_6
#define SPI3_MOSI_GPIO_Port GPIOD
#define SDMMC1_D3_Pin GPIO_PIN_11
#define SDMMC1_D3_GPIO_Port GPIOC
#define FMC_D26_Pin GPIO_PIN_2
#define FMC_D26_GPIO_Port GPIOI
#define FMC_D23_Pin GPIO_PIN_15
#define FMC_D23_GPIO_Port GPIOH
#define FMC_D22_Pin GPIO_PIN_14
#define FMC_D22_GPIO_Port GPIOH
#define FMC_LDQM_Pin GPIO_PIN_0
#define FMC_LDQM_GPIO_Port GPIOE
#define SPI3_SCK_Pin GPIO_PIN_3
#define SPI3_SCK_GPIO_Port GPIOB
#define LTDC_B7_Pin GPIO_PIN_6
#define LTDC_B7_GPIO_Port GPIOK
#define LTDC_B4_Pin GPIO_PIN_3
#define LTDC_B4_GPIO_Port GPIOK
#define SDMMC1_CLK_Pin GPIO_PIN_12
#define SDMMC1_CLK_GPIO_Port GPIOC
#define FMC_D27_Pin GPIO_PIN_3
#define FMC_D27_GPIO_Port GPIOI
#define I2C1_SDA_Pin GPIO_PIN_9
#define I2C1_SDA_GPIO_Port GPIOB
#define I2C1_SCL_Pin GPIO_PIN_8
#define I2C1_SCL_GPIO_Port GPIOB
#define FMC_CAS_Pin GPIO_PIN_15
#define FMC_CAS_GPIO_Port GPIOG
#define LTDC_DE_Pin GPIO_PIN_7
#define LTDC_DE_GPIO_Port GPIOK
#define QSPI_FLASH_BK2_IO3_Pin GPIO_PIN_14
#define QSPI_FLASH_BK2_IO3_GPIO_Port GPIOG
#define LTDC_B2_Pin GPIO_PIN_14
#define LTDC_B2_GPIO_Port GPIOJ
#define LTDC_B0_Pin GPIO_PIN_12
#define LTDC_B0_GPIO_Port GPIOJ
#define SDMMC1_CMD_Pin GPIO_PIN_2
#define SDMMC1_CMD_GPIO_Port GPIOD
#define FMC_D0_Pin GPIO_PIN_0
#define FMC_D0_GPIO_Port GPIOD
#define FMC_D21_Pin GPIO_PIN_13
#define FMC_D21_GPIO_Port GPIOH
#define FMC_D30_Pin GPIO_PIN_9
#define FMC_D30_GPIO_Port GPIOI
#define KEY2_Pin GPIO_PIN_13
#define KEY2_GPIO_Port GPIOC
#define BEEP_Pin GPIO_PIN_8
#define BEEP_GPIO_Port GPIOI
#define LTDC_B1_Pin GPIO_PIN_13
#define LTDC_B1_GPIO_Port GPIOJ
#define FMC_D3_Pin GPIO_PIN_1
#define FMC_D3_GPIO_Port GPIOD
#define SDMMC1_D0_Pin GPIO_PIN_8
#define SDMMC1_D0_GPIO_Port GPIOC
#define SDMMC1_D1_Pin GPIO_PIN_9
#define SDMMC1_D1_GPIO_Port GPIOC
#define FMC_D31_Pin GPIO_PIN_10
#define FMC_D31_GPIO_Port GPIOI
#define FMC_CLK_Pin GPIO_PIN_8
#define FMC_CLK_GPIO_Port GPIOG
#define GTP_RST_Pin GPIO_PIN_7
#define GTP_RST_GPIO_Port GPIOG
#define FMC_A2_Pin GPIO_PIN_2
#define FMC_A2_GPIO_Port GPIOF
#define FMC_A1_Pin GPIO_PIN_1
#define FMC_A1_GPIO_Port GPIOF
#define FMC_A0_Pin GPIO_PIN_0
#define FMC_A0_GPIO_Port GPIOF
#define FMC_BA1_Pin GPIO_PIN_5
#define FMC_BA1_GPIO_Port GPIOG
#define QSPI_FLASH_BK1_CS_Pin GPIO_PIN_6
#define QSPI_FLASH_BK1_CS_GPIO_Port GPIOG
#define LTDC_HSYNC_Pin GPIO_PIN_12
#define LTDC_HSYNC_GPIO_Port GPIOI
#define LTDC_VSYNC_Pin GPIO_PIN_13
#define LTDC_VSYNC_GPIO_Port GPIOI
#define LTDC_CLK_Pin GPIO_PIN_14
#define LTDC_CLK_GPIO_Port GPIOI
#define FMC_A3_Pin GPIO_PIN_3
#define FMC_A3_GPIO_Port GPIOF
#define FMC_BA0_Pin GPIO_PIN_4
#define FMC_BA0_GPIO_Port GPIOG
#define GTP_INT_Pin GPIO_PIN_3
#define GTP_INT_GPIO_Port GPIOG
#define GTP_INT_EXTI_IRQn EXTI3_IRQn
#define FMC_A12_Pin GPIO_PIN_2
#define FMC_A12_GPIO_Port GPIOG
#define LTDC_G7_Pin GPIO_PIN_2
#define LTDC_G7_GPIO_Port GPIOK
#define FMC_A5_Pin GPIO_PIN_5
#define FMC_A5_GPIO_Port GPIOF
#define FMC_A4_Pin GPIO_PIN_4
#define FMC_A4_GPIO_Port GPIOF
#define LTDC_G5_Pin GPIO_PIN_0
#define LTDC_G5_GPIO_Port GPIOK
#define LTDC_G6_Pin GPIO_PIN_1
#define LTDC_G6_GPIO_Port GPIOK
#define QSPI_FLASH_BK1_IO3_Pin GPIO_PIN_6
#define QSPI_FLASH_BK1_IO3_GPIO_Port GPIOF
#define QSPI_FLASH_BK1_IO2_Pin GPIO_PIN_7
#define QSPI_FLASH_BK1_IO2_GPIO_Port GPIOF
#define QSPI_FLASH_BK1_IO0_Pin GPIO_PIN_8
#define QSPI_FLASH_BK1_IO0_GPIO_Port GPIOF
#define LTDC_G4_Pin GPIO_PIN_11
#define LTDC_G4_GPIO_Port GPIOJ
#define FMC_WE_Pin GPIO_PIN_0
#define FMC_WE_GPIO_Port GPIOC
#define QSPI_FLASH_BK1_IO1_Pin GPIO_PIN_9
#define QSPI_FLASH_BK1_IO1_GPIO_Port GPIOF
#define LTDC_G3_Pin GPIO_PIN_10
#define LTDC_G3_GPIO_Port GPIOJ
#define LTDC_G2_Pin GPIO_PIN_9
#define LTDC_G2_GPIO_Port GPIOJ
#define QSPI_FLASH_BK2_IO0_Pin GPIO_PIN_2
#define QSPI_FLASH_BK2_IO0_GPIO_Port GPIOH
#define KEY1_Pin GPIO_PIN_0
#define KEY1_GPIO_Port GPIOA
#define LTDC_R1_Pin GPIO_PIN_0
#define LTDC_R1_GPIO_Port GPIOJ
#define FMC_D7_Pin GPIO_PIN_10
#define FMC_D7_GPIO_Port GPIOE
#define LTDC_G1_Pin GPIO_PIN_8
#define LTDC_G1_GPIO_Port GPIOJ
#define LTDC_G0_Pin GPIO_PIN_7
#define LTDC_G0_GPIO_Port GPIOJ
#define LTDC_R7_Pin GPIO_PIN_6
#define LTDC_R7_GPIO_Port GPIOJ
#define QSPI_FLASH_BK2_IO1_Pin GPIO_PIN_3
#define QSPI_FLASH_BK2_IO1_GPIO_Port GPIOH
#define I2C2_SCL_Pin GPIO_PIN_4
#define I2C2_SCL_GPIO_Port GPIOH
#define I2C2_SDA_Pin GPIO_PIN_5
#define I2C2_SDA_GPIO_Port GPIOH
#define LTDC_R0_Pin GPIO_PIN_15
#define LTDC_R0_GPIO_Port GPIOI
#define LTDC_R3_Pin GPIO_PIN_1
#define LTDC_R3_GPIO_Port GPIOJ
#define FMC_A7_Pin GPIO_PIN_13
#define FMC_A7_GPIO_Port GPIOF
#define FMC_A8_Pin GPIO_PIN_14
#define FMC_A8_GPIO_Port GPIOF
#define FMC_D6_Pin GPIO_PIN_9
#define FMC_D6_GPIO_Port GPIOE
#define FMC_D8_Pin GPIO_PIN_11
#define FMC_D8_GPIO_Port GPIOE
#define FMC_D18_Pin GPIO_PIN_10
#define FMC_D18_GPIO_Port GPIOH
#define FMC_D19_Pin GPIO_PIN_11
#define FMC_D19_GPIO_Port GPIOH
#define FMC_D1_Pin GPIO_PIN_15
#define FMC_D1_GPIO_Port GPIOD
#define FMC_D0D14_Pin GPIO_PIN_14
#define FMC_D0D14_GPIO_Port GPIOD
#define QSPI_FLASH_CLK_Pin GPIO_PIN_2
#define QSPI_FLASH_CLK_GPIO_Port GPIOB
#define FMC_A6_Pin GPIO_PIN_12
#define FMC_A6_GPIO_Port GPIOF
#define FMC_A9_Pin GPIO_PIN_15
#define FMC_A9_GPIO_Port GPIOF
#define FMC_D9_Pin GPIO_PIN_12
#define FMC_D9_GPIO_Port GPIOE
#define FMC_D12_Pin GPIO_PIN_15
#define FMC_D12_GPIO_Port GPIOE
#define LTDC_R6_Pin GPIO_PIN_5
#define LTDC_R6_GPIO_Port GPIOJ
#define FMC_D17_Pin GPIO_PIN_9
#define FMC_D17_GPIO_Port GPIOH
#define FMC_D20_Pin GPIO_PIN_12
#define FMC_D20_GPIO_Port GPIOH
#define LTDC_BL_Pin GPIO_PIN_13
#define LTDC_BL_GPIO_Port GPIOD
#define LED_G_Pin GPIO_PIN_1
#define LED_G_GPIO_Port GPIOB
#define LTDC_R3J2_Pin GPIO_PIN_2
#define LTDC_R3J2_GPIO_Port GPIOJ
#define FMC_RAS_Pin GPIO_PIN_11
#define FMC_RAS_GPIO_Port GPIOF
#define FMC_A10_Pin GPIO_PIN_0
#define FMC_A10_GPIO_Port GPIOG
#define FMC_D5_Pin GPIO_PIN_8
#define FMC_D5_GPIO_Port GPIOE
#define FMC_D10_Pin GPIO_PIN_13
#define FMC_D10_GPIO_Port GPIOE
#define FMC_CS_Pin GPIO_PIN_6
#define FMC_CS_GPIO_Port GPIOH
#define FMC_D16_Pin GPIO_PIN_8
#define FMC_D16_GPIO_Port GPIOH
#define FMC_D15_Pin GPIO_PIN_10
#define FMC_D15_GPIO_Port GPIOD
#define FMC_D14_Pin GPIO_PIN_9
#define FMC_D14_GPIO_Port GPIOD
#define LED_B_Pin GPIO_PIN_3
#define LED_B_GPIO_Port GPIOA
#define LED_R_Pin GPIO_PIN_0
#define LED_R_GPIO_Port GPIOB
#define LTDC_R4_Pin GPIO_PIN_3
#define LTDC_R4_GPIO_Port GPIOJ
#define LTDC_R5_Pin GPIO_PIN_4
#define LTDC_R5_GPIO_Port GPIOJ
#define FMC_A11_Pin GPIO_PIN_1
#define FMC_A11_GPIO_Port GPIOG
#define FMC_D4_Pin GPIO_PIN_7
#define FMC_D4_GPIO_Port GPIOE
#define FMC_D11_Pin GPIO_PIN_14
#define FMC_D11_GPIO_Port GPIOE
#define FMC_CKE_Pin GPIO_PIN_7
#define FMC_CKE_GPIO_Port GPIOH
#define FMC_D13_Pin GPIO_PIN_8
#define FMC_D13_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
