#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "screen.h"
#include "i2c.h"
#include "spi.h"

/* I2C Vars */
//I2C_TARGET  screen;
//I2C_PORT    i2c;

/*SPI Vars*/
SPI_CS_TARGET spi_cs_flash;
SCREEN screen;

I2C_CONF i2c = {
    .f_cpu = 16000000,
    .frequency = 1000000,
    .mode = MODE_MASTER_POL,
    .prescaler = 1
};


int main(){
    i2cInit(&i2c);
    //screenInit(&screen);

    //screenWrite(&screen, "Hello You");
    while(1){
        //i2cWritePol("hello world", 12, 0x078);
        i2cStartPol(0x4E, I2C_WRITE);
        //screenWrite(&screen, "Hello You");
        PORTB ^= (1<<PB0);
        _delay_ms(1);
        PORTD ^= (1<<PD7);
        _delay_ms(1);
        PORTD ^= (1<<PD6);
        _delay_ms(1);
    }
}


//void screenInitVars(I2C_PORT *port, I2C_TARGET *screen){
 //   static uint8_t instruction[] = {0};

//}