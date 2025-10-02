#include "sram.h"
#include "string.h"


void sendInstruction(uint32_t address){
    uint8_t byte = 0x00;
     for(int i=0; i<=3; i++){
        byte = (address >> (3-i)*8);
        SPDR = (uint8_t)byte;
        while(!(SPSR & (1<<SPIF))){} // Clear flag
     }
}



/* Writing One Byte Process:
>> 1byte: command Instruction [SRAM_WRITE]
>> 3bytes: 24 bit address
>> 1byte: Data out
*/
void writeByte(uint8_t c, uint32_t address){
    address = (SRAM_WRITE << 24) | address; // Combine Command instruction to the address data

    START_SPI;                         // CS low
    sendInstruction(address);          // Send instruction + address
    SPDR = (uint8_t)c;        
    while(!(SPSR & (1<<SPIF))){}
    STOP_SPI;                          // CS high
}

void writeString(char *c, uint32_t address){
    START_SPI;                         // CS low
    for (uint8_t i = 0; i < strlen(c); i++){
        SPDR = (uint8_t)c[i];          // start transfer
        while(!(SPSR & (1<<SPIF))){}   // Clear flag by reading the status register
    }
    STOP_SPI;                          // CS high
}


/* Writing One Byte Process:
>> 1byte: command Instruction [SRAM_READ]
>> 3bytes: 24 bit address
>> 1byte: Data out
*/
void readByte(uint8_t *c, uint32_t address){
    address = (SRAM_READ << 24) | address; // Combine Command instruction to the address data
    *c = 0x00;

    START_SPI;                         // CS low
    sendInstruction(address);          // Send instruction + address
    SPDR = 0x00;
    while(!(SPSR & (1<<SPIF))){}       // Check flag to confirm the data is ready to be read.
    *c = SPDR;                       // Get data from buffer
    STOP_SPI;   
}


void readString(uint8_t *c, size_t len, uint32_t address){

}


