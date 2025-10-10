#ifndef ISP_H
#define ISP_H

#include <avr/iom168.h>
#include <avr/sfr_defs.h>
#include <stdint.h>

#define ISP_SCK       PB5
#define ISP_MISO      PB4
#define ISP_MOSI      PB3
#define ISP_CS        PB1

#define ISP_CPOL      0
#define ISP_CPHA      0

#define START_ISP     (PORTB &= ~_BV(ISP_CS))
#define STOP_ISP      (PORTB |=  _BV(ISP_CS))

void ispInit();
void ispPowerUp();
uint8_t ispTransmitByte(uint8_t data);
uint8_t ispProgrammingEnable();
void ispReadSignatureByte();
void ispReadFuseBits();

#endif
