#include "includes.h"
#include "spi.h"
#include "uart.h"
#include "i2c.h"
#include "sram.h"
#include "screen.h"
#include "shared_memory.h"
void sramVarsInit(SPI *spi);

SCREEN            screen;
I2C_CONF          i2c;
SPI_CS_TARGET     spi_cs_flash;
SPI               spi;//
struct SRAM_MAP   sram_map;

void testBuffer_1(){
    for(uint16_t i=0; i<SCREEN_BUFFER_SIZE; i++){
        if(i%2) bufferWrite(0x0F, 0x00, 0x00, i);
    }
}

void testBuffer_2(){
    for(uint16_t i=0; i<SCREEN_BUFFER_SIZE; i++){
        if(!i%2) bufferWrite(0xff, 0x00, 0x00, i);
    }
}

int main(void){
    DATA_DDR |= (1<<DATA_PIN);
    DATA_PORT |= (1<<DATA_PIN);
    /*________Initialize SRAM parameters______*/
    sramVarsInit(&spi);     

    /*________Initialize Protocols______*/
    spiInit(&spi);                             
    i2cInit(&i2c, true);                        
    uartInit();                                 
    
    /*________Initialize Screen______*/
    screenInit(&screen, true);                             

    /*________Initialize Shared Memory System______*/
    sharedMemoryInit();

    
    while(1){
        //DATA_PORT ^= (1<<DATA_PIN);

        //screenWrite(&screen, "hello world");
        testBuffer_1();
        displayGrid();
        //_delay_ms(500);
        //testBuffer_2();
        //displayGrid();
        //spiWritePoll(&spi, "hello world");
        _delay_ms(500);
        
    }
}

void sramVarsInit(SPI *spi){
    static SPI_CS_TARGET cs_reg;
    static SPI_CONF spi_conf;
    static SPI_MODE spi_mode;

    spi_mode.en = true;
    spi_mode.irq = false;
    spi_mode.mode = 0;
    spi_mode.lsbfirst = false;
    spi_mode.prescaler = 8;
    spi_mode.mstr = true;
    spi_conf.mode_conf = &spi_mode;

    cs_reg.CS_DDR = &DDRB;
    cs_reg.CS_PORT = &PORTB;
    cs_reg.CS_PIN = PB1;

    spi->conf = &spi_conf;
    spi->cs_reg = &cs_reg;
}