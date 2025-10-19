#include "includes.h"
#include "isp.h"
#include "spi.h"
#include "uart.h"
#include "avr/pgmspace.h"

char c[100] = {0};
void ispChipErase();

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
  uartWrite_("entering load\n");
  if(programmer->buffer_size == 0 || programmer->buffer == NULL) return;
  uartWrite_("passed load guard\n");

  sprintf(c, "counter: %d | size: %d | number: %d \n", programmer->page_counter , programmer->buffer_size, programmer->page_number);
  uartWrite_(c);

  programmer->page_counter = 0;
  while(programmer->page_counter < 64 && \
    programmer->page_counter < programmer->buffer_size && \
    programmer->page_number < 128){

    uint16_t word = pgm_read_word(&((const uint16_t*)programmer->buffer)[programmer->page_counter]);
    uint8_t low_byte = 0x00;
    uint8_t high_byte = 0x00;
    spiWritePollByte_(0x40);
    spiWritePollByte_(0x00);
    spiWritePollByte_(programmer->page_counter);
    spiWritePollByte_((uint8_t)pgm_read_word(&((const uint16_t*)programmer->buffer)[programmer->page_counter]));

    spiWritePollByte_(0x48);
    spiWritePollByte_(0x00);
    spiWritePollByte_(programmer->page_counter);
    spiWritePollByte_((uint8_t)(pgm_read_word(&((const uint16_t*)programmer->buffer)[programmer->page_counter++]) >> 8));

    low_byte = word & 0x00FF;
    high_byte = (word >>8);

    sprintf(c, "Wrote high_byte: %X | low_byte: %X | Word: %X\n", high_byte, low_byte, word);
    uartWrite_(c);
    _delay_us(100);
  }

  programmer->current_page = ((uint16_t)programmer->page_number << 6);

  spiWritePollByte_(0x4C);
  spiWritePollByte_((programmer->current_page & 0xFF00) >> 8);
  spiWritePollByte_((programmer->current_page & 0x00FF) );
  spiWritePollByte_(0x00);

  programmer->page_number++; 

  sprintf(c, "PAGE NUMBER >>>>>>> %d | PAGE_COUNTER: %d \n", programmer->page_number, programmer->page_counter);
  uartWrite_(c);
  _delay_ms(20); // SAFE WAIT
}

void ispVerifyProgramMemoryPage(PROGRAMMER *programmer, uint16_t page_addr) {
  uartWrite_("entering verify\n");
  if(programmer->buffer_size == 0 || programmer->buffer == NULL) return;
  uartWrite_("passed verify guard\n");
  sprintf(c, "veryfying page addr: %X\n", page_addr);
  uartWrite_(c);

  programmer->page_counter = 0;
  while(programmer->page_counter < 64 && \
    programmer->page_counter < programmer->buffer_size && \
    programmer->page_number < 128){
    uint16_t word = 0x00;
    uint8_t low_byte = 0x00;
    uint8_t high_byte = 0x00;

    spiWritePollByte_(0x28);
    spiWritePollByte_((page_addr & 0xFF00) >> 8);
    spiWritePollByte_((page_addr & 0x00FF) | programmer->page_counter);
    spiReadPollByte_(&high_byte);

    spiWritePollByte_(0x20);
    spiWritePollByte_((page_addr & 0xFF00) >> 8);
    spiWritePollByte_((page_addr & 0x00FF) | programmer->page_counter++);
    spiReadPollByte_(&low_byte);

    word = ((uint16_t)high_byte << 8) | low_byte;
    sprintf(c, "Expected: %lX  |  Verified: %lX\n", (unsigned long)pgm_read_word(&((const uint16_t*)programmer->buffer)[programmer->page_counter - 1]), (unsigned long)word);
    uartWrite_(c);
    _delay_us(100);
  }
  _delay_ms(20); // SAFE WAIT
}

void ispChipErase() {
  spiWritePollByte_(0xAC);
  spiWritePollByte_(0x80);
  spiWritePollByte_(0x00);
  spiWritePollByte_(0x00);
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


void errorHandler(uint8_t err){
    switch (err) {
        case NOT_SYNC: 
            uartWrite_("Error.. <NOT_SYNC\n");
    }
}*/