#include "start.h"


void gpioConfig(){
    //joyStickConf();
    //spiInitPoll();
}


void joyStickConf(){
    /*Pin Data Direction*/
    JOYSTICK_DDR &= ~((1<<JOYSTICK_UP) | (1<<JOYSTICK_DOWN) | (1<<JOYSTICK_LEFT));
    JOYSTICK_RIGHT_DDR &= ~(1<<JOYSTICK_RIGHT);

    JOYSTICK_PORT |=  ((1<<JOYSTICK_UP)|(1<<JOYSTICK_DOWN)|(1<<JOYSTICK_LEFT));
    JOYSTICK_RIGHT_PORT |= (1<<JOYSTICK_RIGHT);

    /*ISR Conf.*/   
    PCIFR |= (1<<PCIF2) | (1<<PCIF1);    // Clear flag
    PCMSK2 |= (1<<PCINT20) | (1<<PCINT19) | (1<<PCINT18);
    PCMSK1 |= (1<<PCINT8);
    PCICR |= (1<< PCIE2) | (1<< PCIE1);
    sei();
}



void seed_prng(void){ srand(0xA5A5 ^ (uint16_t)TCNT0);}