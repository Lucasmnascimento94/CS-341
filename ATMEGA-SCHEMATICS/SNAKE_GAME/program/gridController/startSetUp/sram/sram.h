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

void sramWriteByte(uint8_t data, uint32_t address);
void sramWriteStringPoll(char *data, uint32_t address);

void sramReadByte(uint8_t *data, uint32_t address);
void sramReadString(uint8_t *data, size_t len, uint32_t address);

void sramReadModeRegister();

#endif