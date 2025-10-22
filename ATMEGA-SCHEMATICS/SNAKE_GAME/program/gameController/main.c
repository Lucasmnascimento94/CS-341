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
I2C_CONF i2c; 


int main(){
    i2cInit(&i2c, true);
    screenInit(&screen);
    _delay_ms(100);

    while(1){
        screenWrite(&screen, "HELLO WORLD ");
        _delay_ms(2000);
    }
}