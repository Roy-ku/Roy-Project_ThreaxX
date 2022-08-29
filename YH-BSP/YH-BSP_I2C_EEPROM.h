#ifndef __I2C_EE_H
#define __I2C_EE_H

#include "YH-BSP.h"

/*
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1
 */

/* I2C interrupt requests */
#define EEPROM_I2C_EV_IRQn I2C1_EV_IRQn
#define EEPROM_I2C_ER_IRQn I2C1_ER_IRQn
#define EEPROM_I2C_EV_IRQHandler I2C1_EV_IRQHandler
#define EEPROM_I2C_ER_IRQHandler I2C1_ER_IRQHandler

/* 等待超時時間 */
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

#define I2Cx_TIMEOUT_MAX 300
/* Maximum number of trials for HAL_I2C_IsDeviceReady() function */
#define EEPROM_MAX_TRIALS 300

bsp_Status bsp_I2C_EEPROM_BufferWrite(uint8_t drvaddr, uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
bsp_Status bsp_I2C_EEPROM_ByteWrite(uint8_t drvaddr, uint8_t value, uint8_t WriteAddr);
bsp_Status bsp_I2C_EEPROM_BufferRead(uint8_t drvaddr, uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
bsp_Status bsp_I2C_EEPROM_Test(void);

#endif /* __I2C_EE_H */
