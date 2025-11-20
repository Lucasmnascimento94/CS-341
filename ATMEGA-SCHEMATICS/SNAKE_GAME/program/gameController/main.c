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
    
    //updateGameScreen(&screen, "Snake",100,210);
    
    
    //updateGameScreen(&screen, "Pacman",100, 210);
   

    //updateGameScreen(&screen, "Pong",100, 210);
    

    //updateGameScreen(&screen, "Spacecraft",100,210);
  

    screenWriteAt(&screen, 0, 3, "Hello");
    //_delay_ms(2);
    
    //screenClear(&screen);
    //_delay_ms();
    //_delay_ms();
   
    /*while(1){
        screenWriteDDRAM(&screen, "Hello World ");
        _delay_ms(2000);
    }*/
}