#include "isp.h"
#include "clock.h"
#include <avr/iom168.h>
#include <avr/sfr_defs.h>
#include <stdint.h>
#include <util/delay.h>

void ispInit() {
  // enable SCK, MOSI, CS as output
  DDRB = _BV(SCK) | _BV(MISO) | _BV(MOSI) | _BV(CS);

  // set SS high so master doesn't get overwritten
  // NOTE: But why? PB2 isn't even CS....
  PORTB |= _BV(PB2);

  // set CS high (idle)
  PORTB |= _BV(CS);

  // enable SPE, re-enable master, set pre-scaler to 64
  SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1) | _BV(SPR0);
}

uint8_t ispTransmitByte(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1 << SPIF)))
    ;
  return SPDR;
}

uint8_t ispProgrammingEnable() {
  START_ISP;
  ispTransmitByte(0xAC);
  ispTransmitByte(0x53);
  ispTransmitByte(0x00);
  ispTransmitByte(0x00);
  STOP_ISP;
  return 0;
}

void ispReadSignatureByte() {
  START_ISP;
  ispTransmitByte(0x30);
  ispTransmitByte(0x00);
  ispTransmitByte(0x00);
  ispTransmitByte(0x00);
  STOP_ISP;
}

void ispReadFuseBits() {
  START_ISP;
  ispTransmitByte(0x50);
  ispTransmitByte(0x00);
  ispTransmitByte(0x00);
  ispTransmitByte(0x00);
  STOP_ISP;
}

// following powerup protocl described in 28.8.2 - Serial Programming Algorithm
void ispPowerUp() {
  // set SCK low
  PORTB &= ~_BV(SCK);

  // pulse RESET high two clock cycles
  PORTB |= _BV(CS);
  _delay_us(2 * CLK_CYC_US);
  PORTB &= ~_BV(CS);

  // wait 20ms before enabling serial programming
  _delay_ms(20);
}

void ispReadProgramMemoryHighByte(uint16_t adr) {
  START_ISP;
  ispTransmitByte(0x28);
  ispTransmitByte(adr >> 8);
  ispTransmitByte(adr & 0xFF);
  ispTransmitByte(0x00);
  STOP_ISP;
}

void ispReadProgramMemoryLowByte(uint16_t adr) {
  START_ISP;
  ispTransmitByte(0x20);
  ispTransmitByte(adr >> 8);
  ispTransmitByte(adr & 0xFF);
  ispTransmitByte(0x00);
  STOP_ISP;
}

void ispLoadProgramMemoryPageHighByte(uint8_t data, uint16_t adr) {
  START_ISP;
  ispTransmitByte(0x48);
  ispTransmitByte(0x00);
  ispTransmitByte(adr & 0xFF);
  ispTransmitByte(data);
  STOP_ISP;
}

void ispLoadProgramMemoryPageLowByte(uint8_t data, uint16_t adr) {
  START_ISP;
  ispTransmitByte(0x40);
  ispTransmitByte(0x00);
  ispTransmitByte(adr & 0xFF);
  ispTransmitByte(data);
  STOP_ISP;
}

void ispWriteProgramMemoryPage(uint16_t adr) {
  START_ISP;
  ispTransmitByte(0x4C);
  ispTransmitByte(adr >> 8);
  ispTransmitByte(adr & 0xFF);
  ispTransmitByte(0x00);
  STOP_ISP;
}

void ispChipErase() {
  START_ISP;
  ispTransmitByte(0xAC);
  ispTransmitByte(0x80);
  ispTransmitByte(0x00);
  ispTransmitByte(0x00);
  STOP_ISP;
}
