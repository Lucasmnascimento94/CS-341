#include "isr.h"

/*
JoysStick - UP    >> PD2
JoysStick - DOWN  >> PD3
JoysStick - LEFT  >> PD4
JoysStick - RIGHT >>  PD5
*/

extern SnakeBelly belly;

ISR(PCINT2_vect){
    static uint8_t prev = 0xFF;
    uint8_t now = PIND;
    uint8_t changed = now ^ prev;
    prev = now;

    if(changed & (1<<JOYSTICK_UP)) {belly.direction = UP; belly.walk = true;}
    if(changed & (1<<JOYSTICK_DOWN)) {belly.direction = DOWN; belly.walk = true;}
    if(changed & (1<<JOYSTICK_LEFT)) {belly.direction = LEFT; belly.walk = true;}
    if(changed & (1<<JOYSTICK_RIGHT)) {belly.direction = RIGHT; belly.walk = true;}
}