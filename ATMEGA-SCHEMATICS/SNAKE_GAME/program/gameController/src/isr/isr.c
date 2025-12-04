#include "isr.h"
#include "shared_memory.h"
/*
JoysStick - UP    >> PD2
JoysStick - DOWN  >> PD3
JoysStick - LEFT  >> PD4
JoysStick - RIGHT >>  PD5
*/

char c[30];
uint8_t isr_flag = 0x00;

ISR(PCINT2_vect){
    static uint8_t prev = 0xFF;
    uint8_t now = PIND;
    uint8_t changed = now ^ prev;
    prev = now;

    if(changed & (1<<JOYSTICK_UP)) {isr_flag = WALK_UP;}
    if(changed & (1<<JOYSTICK_DOWN)) {isr_flag = WALK_DOWN;}
    if(changed & (1<<JOYSTICK_LEFT)) {isr_flag = WALK_LEFT;}
}

ISR(PCINT1_vect){
    static uint8_t prev = 0xFF;
    uint8_t now = PINC;
    uint8_t changed = now ^ prev;
    prev = now;
    if(changed & (1<<JOYSTICK_RIGHT)) {isr_flag= WALK_RIGHT;}
}