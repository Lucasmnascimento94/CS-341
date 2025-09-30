#include "opcodes/sram.h"
#include "string.h"


void writeByte(char *c, uint32_t address){
    START_SPI;                         // CS low
    for (uint8_t i = 0; i < strlen(c); i++){
        SPDR = (uint8_t)c[i];          // start transfer
        while(!(SPSR & (1<<SPIF))){}
    }
    STOP_SPI;                          // CS high
}