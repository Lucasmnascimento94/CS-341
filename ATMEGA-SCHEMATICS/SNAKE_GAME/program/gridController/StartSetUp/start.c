#include "start.h"


void gpioConfig(){
    joyStickConf();
    setUpUART();
}


void joyStickConf(){
    /*Pin Data Direction*/
    JOYSTICK_DDR &= ~((1<<JOYSTICK_UP) | (1<<JOYSTICK_DOWN) | (1<<JOYSTICK_LEFT) | (1<<JOYSTICK_RIGHT));
    JOYSTICK_PORT |=  ((1<<JOYSTICK_UP)|(1<<JOYSTICK_DOWN)|(1<<JOYSTICK_LEFT)|(1<<JOYSTICK_RIGHT));

    JOYSTICK_DDR_ &= ~(1<<JOYSTICK_RIGHT);
    JOYSTICK_PORT_ |=  (1<<JOYSTICK_RIGHT);

    /*ISR Conf.*/   
    //PCIFR |= (1<<PCIF2);    // Clear flag
    //PCMSK2 |= (1<<PCINT21) | (1<<PCINT20) | (1<<PCINT19) | (1<<PCINT18);
    //PCICR |= (1<< PCIE2);
    sei();


    /**/
    JOYSTICK_DDR |= (1<<PD6);
}

void setUpUART(){
    UBRR0H = (unsigned char)(MYUBRR>>8);
    UBRR0L = (unsigned char)MYUBRR;

    UCSR0C &= ~((1<<UMSEL01)|(1<<UMSEL00) | (1<<UPM01) | (1<<UPM00)); // Asynchronous Mode
    UCSR0C |= (1<<USBS0);// 2-STOP BITS
    
    UCSR0A &= ~(1<<U2X0); 

    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    /* Set frame format: 8data, 2stop bit */
   
    /*8 BIT CHARACTER*/
    UCSR0B &= ~(1<<UCSZ02);
    UCSR0C |= (1<<UCSZ01) |(1<<UCSZ00);
}

void seed_prng(void){ srand(0xA5A5 ^ (uint16_t)TCNT0);}