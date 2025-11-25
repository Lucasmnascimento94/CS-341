#ifndef SNAKE_H
#define SNAKE_H

#include "includes.h"

// ============================================================================
// GAME GRID CONFIGURATION
// ============================================================================
#define ROWS 8U      
#define COLUMNS 16U 
#define GRID (ROWS * COLUMNS)

// ============================================================================
// MICROCONTROLLER CONFIGURATION
// ============================================================================
// F_CPU defines the CPU clock frequency (how fast the microcontroller runs)
// 16000000UL means 16 million cycles per second (16 MHz)
// The 'UL' means "unsigned long" - this tells the compiler the number type
// This is needed for timing functions and delays to work correctly
#define F_CPU 16000000UL

// ============================================================================
// JOYSTICK HARDWARE CONFIGURATION
// ============================================================================
// PORTD is a specific set of pins (usually pins 0-7) on the ATmega microcontroller
// DDRD is the "Data Direction Register" for port D - it controls whether pins are inputs or outputs
// We use these to read which direction the joystick is being pushed
#define JOYSTICK_PORT PORTD  // The port where joystick signals are read from
#define JOYSTICK_DDR DDRD    // The register that configures joystick pins as inputs


#define JOYSTICK_UP PD2   // Pin number for up direction (to be defined)
#define JOYSTICK_DOWN PD3  
#define JOYSTICK_LEFT PD4 
#define JOYSTICK_RIGHT PD5

// ============================================================================
// SNAKE MOVEMENT DIRECTION CONSTANTS
// ============================================================================
// These define the possible directions the snake can move
#define STALL 0x00 
#define UP    0x01 
#define DOWN  0x02 
#define LEFT  0x03 
#define RIGHT 0x04 

#define LED_PORT PORTC
#define LED_DDR DDRC
#define LED_PIN PC0

// CLOCK_CYCLE is the relationship between the F_CPU and its duration per clock cycle
// F_CPU determines each cycle's length
// 1 / F_CPU determines the period per cycle -> tells you how delays in your code map to the protocol's waveform requirements
// These values refer to the total number of clock cycles that the MCU must hold the DIN data line HIGH. This represents the ideal pulse length before adjusting for I/O overhead ("overhead" refers to the extra cycles spent by the CPU that don't directly contribute to intended delay)
#define CLOCK_CYCLE 6 // CLOCK_CYCLE ((1u / F_CPU) *  1000000) // in Useconds // DECIDE LATER

#define T0H_TOTAL_CLOCK_CYCLE 6 // (0.4u / CLOCK_CYCLE)
#define T0L_TOTAL_CLOCK_CYCLE 14 // (0.85u / CLOCK_CYCLE)
#define T1H_TOTAL_CLOCK_CYCLE 13 // (0.8u / CLOCK_CYCLE)
#define T1L_TOTAL_CLOCK_CYCLE 7 // (0.45 / CLOCK_CYCLE)

// Real Timing = Data Transfering Time - "overhead"
#define T0H (T0H_TOTAL_CLOCK_CYCLE - CLOCK_CYCLE)
#define T0L (T0L_TOTAL_CLOCK_CYCLE - CLOCK_CYCLE)
#define T1H (T1H_TOTAL_CLOCK_CYCLE - CLOCK_CYCLE)
#define T0L (T1L_TOTAL_CLOCK_CYCLE - CLOCK_CYCLE)

#define G 0xFF
#define R 0x00
#define B 0xFF
#define GRB_VALUE 256


void writeZero(void);
void writeOne(void);
void send_byte(uint8_t b);
void send_pixel(uint8_t g, uint8_t r, uint8_t b);
void clear();





// End of header guard - closes the #ifndef block
#endif



