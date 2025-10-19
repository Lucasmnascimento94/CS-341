#include "sram.h"
#include "string.h"
#include "spi.h"

void sendInstruction(uint32_t address){
    uint8_t byte[4] = {};
     for(int i=0; i<4; i++){
        byte[i] = (uint8_t)((address >> ((3 - i) * 8)) & 0xFF);
     }
     spiWritePoll_(byte, 4);
}


/* Writing One Byte Process:
>> 1byte: command Instruction [SRAM_WRITE]
>> 3bytes: 24 bit address
>> 1byte: Data out
*/
void sramWriteByte(SPI *spi, uint8_t c, uint32_t address){
    address = (SRAM_WRITE << 24) | address; // Combine Command instruction to the address data
    spiStart(spi);
    sendInstruction(address); 
    spiWritePoll_(&c, 1);
    spiStop(spi); 
}


/* Writing One Byte Process:
>> 1byte: command Instruction [SRAM_WRITE]
>> 3bytes: 24 bit address
>> 1byte: Data Stream
*/
void sramWriteStringPoll(SPI *spi, char *data, uint32_t address, uint16_t size){
    address = (SRAM_WRITE << 24) | address;
    spiStart(spi);
    sendInstruction(address); 
    spiWritePoll_((uint8_t *)data, size);
    spiStop(spi); 
}


/* Writing One Byte Process:spiWritePoll
>> 1byte: command Instruction [SRAM_READ]
>> 3bytes: 24 bit address
>> 1byte: Data out
*/
void sramReadByte(SPI *spi, uint8_t *c, uint32_t address){
    address = (SRAM_READ << 24) | address; // Combine Command instruction to the address data
    *c = 0x00;

    spiStart(spi);                        // CS low
    sendInstruction(address);          // Send instruction + address
    SPDR = 0x00;
    while(!(SPSR & (1<<SPIF))){}       // Check flag to confirm the data is ready to be read.
    *c = SPDR;                       // Get data from buffer
    spiStop(spi);   
}


void sramReadString(SPI *spi, uint8_t *data, size_t len, uint32_t address){
    address = (SRAM_READ << 24) | address;
    spiStart(spi);
    sendInstruction(address); 
    spiReadPoll_((uint8_t *)data, len);
    spiStop(spi); 
}


void sramReadModeRegister(SPI *spi){
    uint8_t data = 0x00;
    spiStart(spi);                         // CS low
    SPDR = (uint8_t)(SRAM_RDMR & 0xFF);
    while(!(SPSR & (1<<SPIF))){}       // Check flag to confirm the data is ready to be read.
    SPDR = 0xFF;
    while(!(SPSR & (1<<SPIF))){}
    data = SPDR;                       // Get data from buffer
    spiStop(spi);    
}

void sramWriteModeRegister(SPI *spi, uint8_t mode){
    uint8_t data = 0x00;

    if(mode != SRAM_MODE_BYTE && mode != SRAM_MODE_PAGE \
       && mode != SRAM_MODE_SEQU && mode != SRAM_MODE_RESE){
        return;
    }
    spiStart(spi);                         // CS low
    SPDR = (uint8_t)SRAM_WRMR;
    while(!(SPSR & (1<<SPIF))){}       // Check flag to confirm the data is ready to be read.
    SPDR = mode;
    while(!(SPSR & (1<<SPIF))){}
    data = SPDR;                       // Get data from buffer
    spiStop(spi);   
}