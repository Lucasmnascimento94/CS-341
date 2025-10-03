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
void writeByte(uint8_t c, uint32_t address){
    address = (SRAM_WRITE << 24) | address; // Combine Command instruction to the address data
    START_SPI;
    sendInstruction(address); 
    spiWritePoll_(&c, 1);
    STOP_SPI;
}


/* Writing One Byte Process:
>> 1byte: command Instruction [SRAM_WRITE]
>> 3bytes: 24 bit address
>> 1byte: Data Stream
*/
void writeStringpPoll(char *data, uint32_t address){
    address = (SRAM_WRITE << 24) | address;
    START_SPI;
    sendInstruction(address); 
    spiWritePoll_((uint8_t *)data, strlen(data));
    STOP_SPI;
}


/* Writing One Byte Process:spiWritePoll
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


