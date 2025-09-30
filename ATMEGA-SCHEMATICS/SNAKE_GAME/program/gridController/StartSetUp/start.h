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

#define JOYSTICK_PORT_ PORTC
#define JOYSTICK_DDR_ DDRC
#define JOYSTICK_RIGHT PIN0

#define STALL   0x00
#define UP      0x01
#define DOWN    0x02
#define LEFT    0x03
#define RIGHT   0x04


#define LED_PORT PORTC
#define LED_DDR  DDRC
#define LED_PIN  PC0
#define GRID_PIXELS (48u*32u)


/*#############################-- UART --##################################### */
#define FOSC      16000000UL
#define BAUD_RATE 9600UL
#define MYUBRR    (FOSC/(16UL*BAUD_RATE) - 1) 



/*#############################-- SPI --##################################### */
// ===== SPI config =====

// You will find this information at:
// Atmega168 Datasheet - Pg: 174 - 177
// 23A1024/23LC1024 Datasheet Pg: 6
#define SPI_MODE        0       // 0,1,2,3  (SRAM likes 0)
#define SPI_MSBFIRST    0       // 1=MSB first, 0=LSB first
#define SPI_PRESCALER   64       // 2,4,8,16,32,64,128
#define SPI_USE_IRQ     0       // 1=use SPI interrupt, 0=poll
#define SPI_SPE         1       // SPI Enable(1) Disable(0)
#define SPI_MSTR        1       // MSTR: Master(1)/Slave Select(0)

#define SCK             PB5
#define MISO            PB4
#define MOSI            PB3
#define CS              PB1

// ---- mode -> CPOL/CPHA ----
#if   (SPI_MODE==0)
  #define SPI_CPOL 0
  #define SPI_CPHA 0
#elif (SPI_MODE==1)
  #define SPI_CPOL 0
  #define SPI_CPHA 1
#elif (SPI_MODE==2)
  #define SPI_CPOL 1
  #define SPI_CPHA 0
#elif (SPI_MODE==3)
  #define SPI_CPOL 1
  #define SPI_CPHA 1
#else
  #error "SPI_MODE must be 0..3"
#endif

// ---- prescaler -> SPR1/SPR0 + SPI2X ----
#if   (SPI_PRESCALER==2)
  #define SPI_SPR1 0
  #define SPI_SPR0 0
  #define SPI_SPI2X 1
#elif (SPI_PRESCALER==4)
  #define SPI_SPR1 0
  #define SPI_SPR0 0
  #define SPI_SPI2X 0
#elif (SPI_PRESCALER==8)
  #define SPI_SPR1 0
  #define SPI_SPR0 1
  #define SPI_SPI2X 1
#elif (SPI_PRESCALER==16)
  #define SPI_SPR1 0
  #define SPI_SPR0 1
  #define SPI_SPI2X 0
#elif (SPI_PRESCALER==32)
  #define SPI_SPR1 1
  #define SPI_SPR0 0
  #define SPI_SPI2X 1
#elif (SPI_PRESCALER==64)
  #define SPI_SPR1 1
  #define SPI_SPR0 0
  #define SPI_SPI2X 0
#elif (SPI_PRESCALER==128)
  #define SPI_SPR1 1
  #define SPI_SPR0 1
  #define SPI_SPI2X 0
#else
  #error "SPI_PRESCALER must be one of {2,4,8,16,32,64,128}"
#endif

// ---- Interrupt Enable----
#if (SPI_USE_IRQ == 0)
    #define SPI_SPIE 0
#else
    #define SPI_SPIE 1
#endif

#if (SPI_MSBFIRST == 0)
    #define SPI_DORD 0
#else
    #define SPI_DORD 1
#endif



void gpioConfig();
void setUpISR();
void setUpUART();
void joyStickConf();
void setUpSPI();

void seed_prng(void);

#endif