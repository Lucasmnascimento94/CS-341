#include "print.h"
#include "string.h"

void print(char *c, int size){
    for(int i=0; i<strlen(c); i++){
        while(!( UCSR0A & (1<<UDRE0))){
            __builtin_avr_delay_cycles(1);
        }
        UDR0 = ((uint8_t)c[i]);
    }
}