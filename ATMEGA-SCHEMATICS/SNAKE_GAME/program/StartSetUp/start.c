#include "start.h"


void gpioConfig(){
    joyStickConf();
}


void joyStickConf(){
    /*Pin Data Direction*/
    JOYSTICK_DDR &= ~((1<<JOYSTICK_UP) | (1<<JOYSTICK_DOWN) | (1<<JOYSTICK_LEFT) | (1<<JOYSTICK_RIGHT));
    JOYSTICK_PORT |=  ((1<<JOYSTICK_UP)|(1<<JOYSTICK_DOWN)|(1<<JOYSTICK_LEFT)|(1<<JOYSTICK_RIGHT));

    /*ISR Conf.*/
    PCIFR |= (1<<PCIF2);    // Clear flag
    PCMSK2 |= (1<<PCINT21) | (1<<PCINT20) | (1<<PCINT19) | (1<<PCINT18);
    PCICR |= (1<< PCIE2);
    sei();
}