#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>
#include "screen.h"
#include "i2c.h"
#include "spi.h"


SPI_CS_TARGET spi_cs_flash;
SCREEN screen;
I2C_CONF i2c; 


int main(){
    i2cInit(&i2c, true);
    screenInit(&screen, true);
    _delay_ms(100);
    
    

    screenWriteAt(&screen, 0, 1, "Hello! Welcome :)");
    screenWriteAt(&screen, 3, 3, "Let's play!!!");
    _delay_ms(5000);
    
    screenClear(&screen);
    _delay_ms(500);
    //_delay_ms();
   
    updateGameScreen(&screen, "Snake",100,2500);
    _delay_ms(5000);
    
    screenClear(&screen);
    _delay_ms(500);

    updateGameScreen(&screen, "Pac-Man",100, 3000);
    _delay_ms(5000);

    screenClear(&screen);
    _delay_ms(500);

    updateGameScreen(&screen, "Pong",100, 3500);
    _delay_ms(5000);

    screenClear(&screen);
    _delay_ms(500);

    updateGameScreen(&screen, "Spacecraft",100,5000);
    _delay_ms(500);

    /*while(1){
        screenWriteDDRAM(&screen, "Hello World ");
        _delay_ms(2000);
    }*/
}