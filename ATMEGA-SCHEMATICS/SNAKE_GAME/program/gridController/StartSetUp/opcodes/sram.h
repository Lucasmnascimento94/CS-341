#ifndef SRAM_H
#define SRAM_H

#include "start.h"

#define SRAM_READ       0x03 // Read data from memory array beginning at selected address
#define SRAM_WRITE      0X02 // Write data to memory array beginning at selected address
#define SRAM_EDIO       0X3B // Enter Dual I/O access (enter SDI bus mode)
#define SRAM_EQIO       0X38 // Enter Quad I/O access (enter SQI bus mode)
#define SRAM_RSTIO      0XFF // Reset Dual and Quad I/O access (revert to SPI bus mode)
#define SRAM_RDMR       0X05 // Read Mode Register
#define SRAM_WRMR       0X01 // Write Mode Register
#define START_SPI       (PORTB &= ~(1<<CS))
#define STOP_SPI        (PORTB |=  (1<<CS))
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


void writeByte(char *c, uint32_t address);
#endif