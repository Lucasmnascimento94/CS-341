#include "uart.h"
#include "string.h"

void uartWrite(char *c, uint16_t size){
    for(uint16_t i=0; i<size; i++){
        while(!( UCSR0A & (1<<UDRE0))){
            __builtin_avr_delay_cycles(1);
        }
        UDR0 = ((uint8_t)c[i]);
    }
}


void uartWrite_(char *c){
    for(uint16_t i=0; i<strlen(c); i++){
        while(!( UCSR0A & (1<<UDRE0))){
            __builtin_avr_delay_cycles(1);
        }
        UDR0 = ((uint8_t)c[i]);
    }
}
void uartRead(char *c){
    
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