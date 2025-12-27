#include "isr.h"
#include "spi.h"
/*
JoysStick - UP    >> PD2
JoysStick - DOWN  >> PD3
JoysStick - LEFT  >> PD4
JoysStick - RIGHT >>  PD5
*/

char c[30];
char *spi_buffer;

ISR(PCINT2_vect){

}

ISR(SPI_STC_vect){
    
}

void ISR_SPI_CONF(char *buffer){
    spi_buffer = buffer;
}