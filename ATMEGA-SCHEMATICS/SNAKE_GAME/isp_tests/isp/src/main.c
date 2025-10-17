#include "isp.h"
#include <stdint.h>
#include <util/delay.h>

uint16_t PAGE1[64] = {
    0x0C94, 0x3400, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00,
    0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00,
    0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00,
    0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00,
    0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00,
    0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00,
    0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x1124, 0x1FBE, 0xCFEF, 0xD4E0,
    0xDEBF, 0xCDBF, 0x0E94, 0x4000, 0x0C94, 0x4F00, 0x0C94, 0x0000,
};

uint16_t PAGE2[64] = {
    0x509A, 0x91E0, 0x8BB1, 0x8927, 0x8BB9, 0x2FEF, 0x34E3, 0x8CE0,
    0x2150, 0x3040, 0x8040, 0xE1F7, 0x00C0, 0x0000, 0xF3CF, 0xF894,
    0xFFCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
};

void writeBlinky();
void verifyBlinky();

int main() {
  // skipping this powerup reset stuff for now... it works, ok...
  // DDRB |= (1 << PB5);
  // PORTB &= ~(1 << PB5);
  //
  // DDRB |= (1 << PB1);
  // PORTB &= ~(1 << PB1);
  // PORTB |= (1 << PB1);
  // _delay_ms(5);
  // PORTB &= ~(1 << PB1);
  // _delay_ms(25);
  DDRB |= (1 << PB1);
  PORTB &= ~(1 << PB1);
  _delay_ms(1);
  ispInit();
  // not sure if this is actually needed - will return to it: see 28.8.2, p304
  // ispPowerUp();
  START_ISP;
  _delay_ms(100); // avrdude / usbasp seems to do this
  writeBlinky();
  verifyBlinky();
  STOP_ISP;
  while (1)
    ;
}

void writeBlinky() {
  ispProgrammingEnable();
  ispChipErase();
  _delay_ms(10); // see minimum delays 28.8.2 p305

  // arvdude / usbasp seems to do this
  STOP_ISP;
  _delay_us(100);
  START_ISP;
  _delay_ms(100);

  ispProgrammingEnable();

  // have to load a full page using Load Program Memory Page Low/High Byte...
  for (uint8_t i = 0; i < 64; ++i) {
    ispLoadProgramMemoryPageLowByte(PAGE1[i] >> 8, 0x00 + i);
    ispLoadProgramMemoryPageHighByte(PAGE1[i] & 0xFF, 0x00 + i);
  }
  // ...then write it with address of final byte (maybe the page's address?)
  ispWriteProgramMemoryPage(0x003F);

  _delay_ms(5); // see minimum delays 28.8.2 p305

  for (uint8_t i = 0; i < 64; ++i) {
    ispLoadProgramMemoryPageLowByte(PAGE2[i] >> 8, 0x40 + i);
    ispLoadProgramMemoryPageHighByte(PAGE2[i] & 0xFF, 0x40 + i);
  }
  ispWriteProgramMemoryPage(0x007F);
}

void verifyBlinky() {
  for (uint8_t i = 0; i < 64; ++i) {
    ispReadProgramMemoryLowByte(0x00 + i);
    ispReadProgramMemoryHighByte(0x00 + i);
  }
  for (uint8_t i = 0; i < 64; ++i) {
    ispReadProgramMemoryLowByte(0x40 + i);
    ispReadProgramMemoryHighByte(0x40 + i);
  }
}
