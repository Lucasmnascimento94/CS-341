#ifndef ISP_H
#define ISP_H

#include <avr/iom168.h>
#include <avr/sfr_defs.h>
#include <stdint.h>

#define SCK       PB5
#define MISO      PB4
#define MOSI      PB3
#define CS        PB1

#define START_ISP     (PORTB &= ~_BV(CS))
#define STOP_ISP      (PORTB |=  _BV(CS))

void ispInit();
void ispPowerUp();
uint8_t ispTransmitByte(uint8_t data);
uint8_t ispProgrammingEnable();
void ispChipErase();
void ispLoadProgramMemoryPageHighByte(uint16_t adr, uint8_t data);
void ispLoadProgramMemoryPageLowByte(uint16_t adr, uint8_t data);
void ispReadProgramMemoryHighByte(uint16_t adr);
void ispReadProgramMemoryLowByte(uint16_t adr);
void ispReadSignatureByte(uint8_t adr);
void ispReadFuseBits();
void ispWriteProgramMemoryPage(uint16_t adr);

#endif
