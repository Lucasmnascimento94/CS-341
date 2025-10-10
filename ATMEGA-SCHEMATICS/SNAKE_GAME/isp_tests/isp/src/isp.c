#include "isp.h"
#include <avr/sfr_defs.h>
#include <stdint.h>
#include <util/delay.h>

void ispInit() {
  // same setup as spiInitPoll, just wanted to make sure i understood it
  DDRB = _BV(ISP_SCK) | _BV(MSTR) | _BV(ISP_MOSI) | _BV(ISP_CS);
  PORTB |= _BV(PB2);
  PORTB |= _BV(ISP_CS);
  SPCR = _BV(SPE) | _BV(MSTR) | _BV(ISP_CPOL) | _BV(ISP_CPHA) | _BV(SPR1);
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
    _delay_us(320);
    STOP_ISP;
    _delay_us(320);
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
  DDRB |= ~_BV(ISP_SCK);

  // pulse RESET high two clock cycles
  DDRB &= _BV(ISP_CS);
  // see: asm-gcc inline assembler cookbook
  __asm__ volatile("nop\n\t"
                   "nop\n\t" ::);
  DDRB |= ~_BV(ISP_CS);

  // wait 20ms before enabling serial programming
  _delay_ms(20);
}
