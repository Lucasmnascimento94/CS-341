#ifndef START_H
#define START_H


#include "includes.h"

/*
JoyStick_UP    >> PD2 - PCINT18
JoyStick_DOWN  >> PD3 - PCINT19
JoyStick_LEFT  >> PD4 - PCINT20
JoyStick_RIGHT >> PD5 - PCINT21

*/

#define F_CPU 16000000UL

#define JOYSTICK_PORT PORTD
#define JOYSTICK_DDR DDRD
#define JOYSTICK_UP   PIN2
#define JOYSTICK_DOWN PIN3
#define JOYSTICK_LEFT PIN4
#define JOYSTICK_RIGHT PIN5


#define UP 0x00
#define DOWN 0x01
#define LEFT 0x10
#define RIGHT 0x11


#define LED_PORT PORTC
#define LED_DDR  DDRC
#define LED_PIN  PC0
#define GRID_PIXELS (40u*32u)
#define SNAKE_SIZE_INIT 10
#define IO_WRITE_CYC 6


#ifndef CYCLE_NS
  #if !defined(F_CPU)
    #error "F_CPU must be defined before including this header"
  #endif

  #if   (F_CPU == 16000000UL)
    #define T1H_TOT 13  
    #define T1L_TOT 7    
    #define T0H_TOT 6    
    #define T0L_TOT 12   
    #define TLL 10000
  #elif (F_CPU == 20000000UL)
    #define T1H_TOT 14   // 0.70us high
    #define T1L_TOT 9   // 0.55us low
    #define T0H_TOT 7    // 0.35us high
    #define T0L_TOT 18   // 0.90us low
    #define TLL 100
  #else
    #define CYCLE_NS (1000000000UL / (F_CPU)) /* generic fallback */
  #endif
#endif

#define T1H (T1H_TOT - IO_WRITE_CYC)
#define T1L (T1L_TOT - IO_WRITE_CYC)
#define T0H (T0H_TOT - IO_WRITE_CYC)
#define T0L (T0L_TOT - IO_WRITE_CYC)

#define COLOR_G 0xFF
#define  COLOR_R 0x00
#define  COLOR_B 0xFF

void gpioConfig();
void setUpISR();
void joyStickConf();

#endif