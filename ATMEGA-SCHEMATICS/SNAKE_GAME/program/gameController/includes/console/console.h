#ifndef START_H
#define START_H
#include "includes.h"


/*
JoyStick_UP    >> PD2 - PCINT18
JoyStick_DOWN  >> PD3 - PCINT19
JoyStick_LEFT  >> PD4 - PCINT20
JoyStick_RIGHT >> PC0 - PCINT8

*/

#define JOYSTICK_PORT PORTD
#define JOYSTICK_DDR DDRD
#define JOYSTICK_UP   PD2
#define JOYSTICK_DOWN PD3
#define JOYSTICK_LEFT PD4

#define JOYSTICK_RIGHT_DDR DDRC
#define JOYSTICK_RIGHT_PORT PORTC
#define JOYSTICK_RIGHT PC0


void consoleConf();
void enableReg();
void seed_prng(void);

#endif