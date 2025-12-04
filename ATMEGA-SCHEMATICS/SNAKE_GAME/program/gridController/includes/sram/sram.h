#ifndef SRAM_H
#define SRAM_H

#include "start.h"

#define SRAM_READ           (uint32_t)0x03 // Read data from memory array beginning at selected address
#define SRAM_WRITE          (uint32_t)0X02 // Write data to memory array beginning at selected address
#define SRAM_EDIO           (uint32_t)0X3B // Enter Dual I/O access (enter SDI bus mode)
#define SRAM_EQIO           (uint32_t)0X38 // Enter Quad I/O access (enter SQI bus mode)
#define SRAM_RSTIO          (uint32_t)0XFF // Reset Dual and Quad I/O access (revert to SPI bus mode)
#define SRAM_RDMR           (uint32_t)0X05 // Read Mode Register
#define SRAM_WRMR           (uint32_t)0X01 // Write Mode Register


#define SRAM_MODE_BYTE      (uint8_t)(0x00)
#define SRAM_MODE_PAGE      (uint8_t)(0x00 | (0x02<<6))
#define SRAM_MODE_SEQU      (uint8_t)(0x00 | (0x01<<6))
#define SRAM_MODE_RESE      (uint8_t)(0x00 | (0x03<<6))

/* Data Sample TIming

Data Sample -> Rising
SetUp       -> Falling

*/


/*#########################-- READING FROM SRAM --############################
############################################################################*/

/* Read Byte

1 - Instruction: SRAM_READ
2 - ADDRESS: 3 Bytes Address
3-  Read Data

*/


/*#########################-- WRITING ON SRAM --############################
############################################################################*/

/* Write Byte

1 - Instruction: SRAM_WRITE
2 - ADDRESS: 3 Bytes Address
3 - Write Byte

*/

void sendInstruction(uint32_t address);

void sramWriteByte(SPI *spi, uint8_t data, uint32_t address);
void sramWriteU16(SPI *spi, uint16_t data, uint32_t address);
void sramWriteU32(SPI *spi, uint32_t data, uint32_t address);
void sramWriteStringPoll(SPI *spi, char *data, uint32_t address, uint16_t size);

void sramReadByte(SPI *spi, uint8_t *data, uint32_t address);
void sramReadU16(SPI *spi, uint16_t *data, uint32_t address);
void sramReadU32(SPI *spi, uint32_t *data, uint32_t address);
void sramReadString(SPI *spi, uint8_t *data, size_t len, uint32_t address);
void sramReadBuffer(SPI *spi, uint32_t size, uint32_t addr_start);

uint8_t sramReadModeRegister(SPI *spi);
void sramWriteModeRegister(SPI *spi, uint8_t mode);
#endif