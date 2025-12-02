#ifndef START_H
#define START_H


#include "includes.h"
#include "spi.h"
#include "uart.h"

/*
JoyStick_UP    >> PD2 - PCINT18
JoyStick_DOWN  >> PD3 - PCINT19
JoyStick_LEFT  >> PD4 - PCINT20
JoyStick_RIGHT >> PD5 - PCINT21

*/


#define STALL   0x00
#define UP      0x01
#define DOWN    0x02
#define LEFT    0x03
#define RIGHT   0x04


#define LED_PORT PORTC
#define LED_DDR  DDRC
#define LED_PIN  PC0
#define GRID_PIXELS (48u*32u)


void joyStickConf();
void seed_prng(void);

#endif