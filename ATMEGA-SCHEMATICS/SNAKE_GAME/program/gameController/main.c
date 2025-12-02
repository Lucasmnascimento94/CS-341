#include "includes.h"
#include "spi.h"
//#include "shared_memory.h"
#include "data_types.h"
#include "uart.h"
#include "i2c.h"
#include "screen.h"
void sramVarsInit(SPI *spi);


SPI_CS_TARGET spi_cs_flash;
SPI          spi;
//struct SRAM_MAP sram_map;

I2C_CONF i2c;
SCREEN screen;

uint8_t val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

int main(){

    i2cInit(&i2c, true);
    screenInit(&screen, true);

    //sram_map.cell.block_size = 16;
    //char c[100];
    //sramVarsInit(&spi);
    //setUpUART();
    //spiInit(&spi);
    _delay_ms(100);
//    struct Cell cell;
//    struct Cell temp;
/*
    uartWrite_(c);
    _delay_ms(1000);
    for(uint32_t i=0; i < 0x1330*4; i++){
        sramWriteByte(&spi, NULL_PTR, i);
    }

    _delay_ms(1000);
    temp = cell;
    for(int i=0; i<500; i++){
        cell.val = i;
        pushCell(&cell);
    }

    _delay_ms(1000);

    readCell(&cell, sram_map.cell.head_base);
    for(; cell.next_addr != NULL_PTR; readCell(&cell, cell.next_addr)){
        sprintf(c, "PREV: %lX | NEXT: %lX | VAL: %d\n", \
            cell.prev_addr\
            ,cell.next_addr\
            ,cell.val);
        uartWrite_(c);
        
    }

    //_delay_ms(500);
    //spiWritePoll(&spi, "HELLO WORLD");
*/
_delay_ms(1000);

    while(1){
        screenWrite(&screen, "HELLO WORLD UMB");
        //i2cStartPol(screen.pcf8574_addr, MODE_MASTER_POL);
        //i2cWritePol_("Hello World", 10);
        //i2cWritePol_("Hello World", 11);
        //i2cStop();
        _delay_ms(1000);
        //uartWrite_("MAIN LOOP\n");
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
    spi_mode.prescaler = 2;
    spi_mode.mstr = true;
    spi_conf.mode_conf = &spi_mode;

    cs_reg.CS_DDR = &DDRB;
    cs_reg.CS_PORT = &PORTB;
    cs_reg.CS_PIN = PB1;

    spi->conf = &spi_conf;
    spi->cs_reg = &cs_reg;
}
