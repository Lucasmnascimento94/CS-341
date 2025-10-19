#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/interrupt.h>

int main(){
    DDRB |= (1<<PB0);
    PORTB |= (1<<PB0);
    DDRD |= (1<<PD7) | (1<<PD6);
    PORTD |= (1<<PD7) | (1<<PD6);

    char msg = "HELLO WORLD WHERE AM I?\n";

    while(1){
        PORTB ^= (1<<PB0);
        _delay_ms(1);
        PORTD ^= (1<<PD7);
        _delay_ms(1);
        PORTD ^= (1<<PD6);
        _delay_ms(1);
    }
}