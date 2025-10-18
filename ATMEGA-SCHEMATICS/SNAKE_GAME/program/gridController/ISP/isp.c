#include "includes.h"
#include "isp.h"
#include "spi.h"
#include "uart.h"


uint8_t ispInit(TARGET *target){
  uint8_t try = 0x00;
  do{
      powerUp();
      ispProgrammingEnable(target);
      
  }
  while(target->status != 0x53 && try++ < 10);

  return (target->status == 0x53 )?0x01:0x00;
}

void powerUp(){
  SPI_PORT &= ~(1<<CS) & ~(1<<SCK);
  __builtin_avr_delay_cycles(10);
  SPI_PORT |= (1<<CS);
  __builtin_avr_delay_cycles(10);
  SPI_PORT &= ~(1<<CS);
  _delay_ms(100);
}

void ispProgrammingEnable(TARGET *target) {
  spiWritePollByte_(0xAC);
  spiWritePollByte_(0x53);
  spiReadPollByte_(&(target->status));
  spiWritePollByte_(0x00);
}

void ispReadSignatureByte(TARGET *target, uint8_t signature) {
  spiWritePollByte_(0x30);
  spiWritePollByte_(0x00);
  spiWritePollByte_(signature);

  switch (signature) {
    case SIGNATURE_VENDOR:
        spiReadPollByte_(&(target->signature_vendor));
        break;
    case SIGNATURE_FAMILY:
        spiReadPollByte_(&(target->signature_family));
        break;
    case SIGNATURE_NUMBER:
        spiReadPollByte_(&(target->signature_number));
        break;
  }
}


void ispReadFuseBits(TARGET *target, uint8_t fuse) {
    uint8_t bytes[2];

    if(fuse == HFUSE){bytes[0] = 0x58;}
    else{bytes[0] = 0x50;}

    if(fuse == LFUSE){bytes[1] = 0x00;}
    else{bytes[1] = 0x08;}

    spiWritePollByte_(bytes[0]);
    spiWritePollByte_(bytes[1]);
    spiWritePollByte_(0x00);

    switch (fuse){
        case LFUSE:
            spiReadPollByte_(&(target->lfuse));
            break;
        case HFUSE:
            spiReadPollByte_(&(target->hfuse));
            break;
        case EXTFUSE:
            spiReadPollByte_(&(target->exfuse));
            break;
    }
}

void ispLoadProgramMemoryPage(PROGRAMMER *programmer) {
  if(programmer->buffer_size == 0 || programmer->buffer == NULL) return;

  while(programmer->page_counter < 64 && \
    programmer->page_counter < programmer->buffer_size && \
    programmer->page_number < 128){
    spiWritePollByte_(0x40);
    spiWritePollByte_(0x00);
    spiWritePollByte_(programmer->page_counter);
    spiWritePollByte_((uint8_t)(programmer->buffer[programmer->page_counter]));

    spiWritePollByte_(0x48);
    spiWritePollByte_(0x00);
    spiWritePollByte_(programmer->page_counter);
    spiWritePollByte_((uint8_t)(programmer->buffer[programmer->page_counter++] >> 8));
  }

  programmer->current_page = ((uint16_t)programmer->page_number << 6);

  spiWritePollByte_(0x4C);
  spiWritePollByte_((programmer->current_page & 0xFF00) >> 8);
  spiWritePollByte_((programmer->current_page & 0x00FF) );
  spiWritePollByte_(0x00);

  programmer->page_number++; 
  programmer->page_counter = 0;
  _delay_ms(20); // SAFE WAIT
}

/*
void ispReadProgramMemoryHighByte(uint16_t adr) {
  ispTransmitByte(0x28);
  ispTransmitByte(adr >> 8);
  ispTransmitByte(adr & 0xFF);
  ispTransmitByte(0x00);
}

void ispReadProgramMemoryLowByte(uint16_t adr) {
  ispTransmitByte(0x20);
  ispTransmitByte(adr >> 8);
  ispTransmitByte(adr & 0xFF);
  ispTransmitByte(0x00);
}

void ispLoadProgramMemoryPageHighByte(uint8_t data, uint16_t adr) {
  ispTransmitByte(0x48);
  ispTransmitByte(0x00);
  ispTransmitByte(adr & 0xFF);
  ispTransmitByte(data);
}

void ispLoadProgramMemoryPageLowByte(uint8_t data, uint16_t adr) {
  ispTransmitByte(0x40);
  ispTransmitByte(0x00);
  ispTransmitByte(adr & 0xFF);
  ispTransmitByte(data);
}

void ispWriteProgramMemoryPage(uint16_t adr) {
  ispTransmitByte(0x4C);
  ispTransmitByte(adr >> 8);
  ispTransmitByte(adr & 0xFF);
  ispTransmitByte(0x00);
}

void ispChipErase() {
  ispTransmitByte(0xAC);
  ispTransmitByte(0x80);
  ispTransmitByte(0x00);
  ispTransmitByte(0x00);
}


void errorHandler(uint8_t err){
    switch (err) {
        case NOT_SYNC: 
            uartWrite_("Error.. <NOT_SYNC\n");
    }
}*/