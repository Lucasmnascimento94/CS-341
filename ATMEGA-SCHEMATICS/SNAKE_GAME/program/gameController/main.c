#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "screen.h"
#include "i2c.h"
#include "spi.h"

/* I2C Vars */
I2C_TARGET  screen;
I2C_PORT    i2c;

/*SPI Vars*/
SPI_CS_TARGET spi_cs_flash;



int main(){


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


void screenInitVars(I2C_PORT *port, I2C_TARGET *screen){
    static uint8_t instruction[] = {0};

}