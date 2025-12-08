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

#define JOYSTICK_PORT PORTD
#define JOYSTICK_DDR DDRD
#define JOYSTICK_UP   PD2
#define JOYSTICK_DOWN PD3
#define JOYSTICK_LEFT PD4
// V6 CONSOLE.H
#define JOYSTICK_RIGHT_DDR DDRC
#define JOYSTICK_RIGHT_PORT PORTC
#define JOYSTICK_RIGHT PC0

#define STALL   0x00
#define UP      0x01
#define DOWN    0x02
#define LEFT    0x03
#define RIGHT   0x04


// #define LED_PORT PORTC
// #define LED_DDR  DDRC
// #define LED_PIN  PC0
#define GRID_PIXELS (8u*32u)


void gpioConfig();
void setUpISR();
void setUpUART();
void joyStickConf();
void setUpSPI();
void spiInitPoll();
void enableReg();
void seed_prng(void);

#endif