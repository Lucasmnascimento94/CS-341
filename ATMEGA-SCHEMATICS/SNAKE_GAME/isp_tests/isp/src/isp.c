#include "clock.h"
#include "isp.h"
#include <avr/iom168.h>
#include <avr/sfr_defs.h>
#include <stdint.h>
#include <util/delay.h>

void ispInit() {
  // enable SCK, MOSI, CS as output
  DDRB = _BV(SCK) |  _BV(MOSI) | _BV(CS);

  // set SS high so master doesn't get overwritten
  // NOTE: But why? PB2 isn't even CS....
  PORTB |= _BV(PB2);

  // set CS high (idle)
  PORTB |= _BV(CS);

  // enable SPE, re-enable master, set pre-scaler to 64
  SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1);
}

uint8_t ispTransmitByte(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1 << SPIF)))
    ;
  return SPDR;
}

uint8_t ispProgrammingEnable() {
  // same as what usbasp does, just using builtin timer
  // after sending third byte, slave echoes 0x53 if successfully enabled
  uint8_t check = 0;
  uint8_t checks = 32;
  while (checks--) {
    ispTransmitByte(0xAC);
    ispTransmitByte(0x53);
    check = ispTransmitByte(0x00);
    ispTransmitByte(0x00);

    if (check == 0x53) {
      return 0;
    }

    // if failed pulse CS and retry
    _delay_us(CLK_CYC_US);
    STOP_ISP; // takes 1.95us
    _delay_us(CLK_CYC_US);
    START_ISP;
  }

  return 1;
}

void ispReadSignatureByte() {
  ispTransmitByte(0x30);
  ispTransmitByte(0x00);
  ispTransmitByte(0x00);
  ispTransmitByte(0x00);
}

void ispReadFuseBits() {
  ispTransmitByte(0x50);
  ispTransmitByte(0x00);
  ispTransmitByte(0x00);
  ispTransmitByte(0x00);
}

// following powerup protocl described in 28.8.2 - Serial Programming Algorithm
void ispPowerUp() {
  // set SCK low
  DDRB |= ~_BV(SCK);

  // pulse RESET high two clock cycles
  DDRB &= _BV(CS);
  _delay_us(2 * CLK_CYC_US);
  DDRB |= ~_BV(CS);

  // wait 20ms before enabling serial programming
  _delay_ms(20);
}

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
  ispTransmitByte(adr >> 4);
  ispTransmitByte(adr & 0xFF);
  ispTransmitByte(0x00);
}

void ispChipErase() {
  ispTransmitByte(0xAC);
  ispTransmitByte(0x80);
  ispTransmitByte(0x00);
  ispTransmitByte(0x00);
}
