#include "isp.h"
#include <stdint.h>
#include <util/delay.h>

#define PAGESIZE 64

uint16_t PAGE1[PAGESIZE] = {
    0x0C94, 0x3400, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00,
    0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00,
    0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00,
    0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00,
    0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00,
    0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00,
    0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x1124, 0x1FBE, 0xCFEF, 0xD4E0,
    0xDEBF, 0xCDBF, 0x0E94, 0x4000, 0x0C94, 0x4F00, 0x0C94, 0x0000,
};

uint16_t PAGE2[PAGESIZE] = {
    0x509A, 0x91E0, 0x8BB1, 0x8927, 0x8BB9, 0x2FEF, 0x34E3, 0x8CE0,
    0x2150, 0x3040, 0x8040, 0xE1F7, 0x00C0, 0x0000, 0xF3CF, 0xF894,
    0xFFCF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
};

void writePage(uint16_t base_adr, uint16_t data[PAGESIZE]);
void verifyPage(uint16_t base_adr);
void writeBlinky();

int main() {
  DDRB |= _BV(CS);
  PORTB &= ~_BV(CS);
  _delay_ms(1);
  ispInit();

  START_ISP;
  _delay_ms(100); // avrdude / usbasp seems to do this
  writeBlinky();
  STOP_ISP;

  while (1)
    ;
}

void writePage(uint16_t base_adr, uint16_t data[PAGESIZE]) {
  // have to load a full page using Load Program Memory Page Low/High Byte...
  for (uint8_t i = 0; i < PAGESIZE; ++i) {
    // it looks like the values are swapped, but that's just how they're stored
    ispLoadProgramMemoryPageLowByte(base_adr + i, data[i] >> 8);
    ispLoadProgramMemoryPageHighByte(base_adr + i, data[i] & 0xFF);
  }
  // ...then write it with address of final byte
  ispWriteProgramMemoryPage(base_adr + PAGESIZE - 1);
  _delay_ms(5); // see minimum delays 28.8.2 p305
}

void verifyPage(uint16_t base_adr) {
  // TODO: actually do something with this
  for (uint8_t i = 0; i < PAGESIZE; ++i) {
    ispReadProgramMemoryLowByte(base_adr + i);
    ispReadProgramMemoryHighByte(base_adr + i);
  }
}

void writeBlinky() {
  ispProgrammingEnable();

  // don't actually need to do this, but here it is
  ispReadSignatureByte(0x00);
  ispReadSignatureByte(0x01);
  ispReadSignatureByte(0x02);

  ispChipErase();
  _delay_ms(10); // see minimum delays 28.8.2 p305

  // arvdude / usbasp seems to do this
  STOP_ISP;
  _delay_us(100);
  START_ISP;
  _delay_ms(100);

  ispProgrammingEnable();

  // eventually we want to do this dynamically (obviously)
  writePage(0x00, PAGE1);
  verifyPage(0x00);
  writePage(0x40, PAGE2);
  verifyPage(0x40);
}

