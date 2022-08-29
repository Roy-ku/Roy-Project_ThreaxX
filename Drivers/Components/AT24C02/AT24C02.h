#ifndef __AT24C02_H
#define __AT24C02_H

#include "YH-BSP.h"

#define AT24C02_ADDRESS 0xA0
#define AT24C02_PAGESIZE 8

typedef struct
{
  uint8_t (*DrvAddr)(void);
  uint8_t (*PageSize)(void);
  uint8_t (*ByteWrite)(uint8_t value, uint8_t WriteAddr);
  uint8_t (*BufferRead)(uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
  uint8_t (*BufferWrite)(uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);

} EEPROM_DrvTypeDef;

extern EEPROM_DrvTypeDef at24c02_drv;

uint8_t at24c02_DevAddr(void);
uint8_t at24c02_PageSize(void);
uint8_t at24c02_ByteWrite(uint8_t value, uint8_t WriteAddr);
uint8_t at24c02_BufferRead(uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
uint8_t at24c02_BufferWrite(uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
#endif /* __AT24C02_H */
