#include "includes.h"
#include "spi.h"
#include "uart.h"
#include "sram.h"
#include "shared_memory.h"
void init();
void sramVarsInit(SPI *spi);

SPI_CS_TARGET     spi_cs_flash;
SPI               spi;//
struct SRAM_MAP   sram_map;

void testBuffer_1(){
    for(uint32_t i=0; i<SCREEN_BUFFER_SIZE; i++){
        uint32_t color_ = COLOR_RED;
        if(i%2 == 0) bufferWrite((color_ >> 16) & 0xff, (color_ >> 8) & 0xff, color_  & 0xff, i);
        else bufferWrite(0x00, 0x00, 0x00, i);
    }
}

void testBuffer_2(){
    for(uint32_t i=0; i<SCREEN_BUFFER_SIZE; i++){
        uint32_t color_ = COLOR_RED;
        if(i%2 != 0) bufferWrite((color_ >> 16) & 0xff, (color_ >> 8) & 0xff, color_  & 0xff, i);
        else bufferWrite(0x00, 0x00, 0x00, i);
    }
}

int main(void){
    init();
    while(1){
        /*___________Wait for Clear_To_Access signal__________*/
        memAcquire();
        /*________Control SPI data bus to access sram_________*/
        spiResume(&spi);
        testBuffer_1();
        displayGrid();
        //_delay_ms(1000);

        testBuffer_2();
        /*_________Read Commands_________*/
        /*_________Read Buffer_________*/
        /*_________Free RAM ans data bus_________*/

        displayGrid();

        //_delay_ms(1000);
        bufferClear();
        displayGrid();
        //_delay_ms(1000);

        spiPause(&spi);
        memFree();
        _delay_us(2);
    }
}

void init(){
    /*________Initialize SRAM parameters______*/
    sramVarsInit(&spi);     

    /*________Initialize Protocols______*/
    spiInit(&spi);                                                 
    //uartInit();
    ws2812bInit();                                 
    
    memAcquire();
    /*________Enforce SRAM Sequencial Mode______*/
    sramWriteModeRegister(&spi, SRAM_MODE_SEQU);                         

    /*________Initialize Shared Memory System______*/
    sharedMemoryInit();

    memFree();
    _delay_ms(7);
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