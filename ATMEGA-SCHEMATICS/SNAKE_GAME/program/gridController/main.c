#include "includes.h"
#include "spi.h"
#include "uart.h"
#include "i2c.h"
#include "sram.h"
#include "screen.h"
#include "shared_memory.h"
void sramtesting();
void sramVarsInit(SPI *spi);

SCREEN            screen;
I2C_CONF          i2c;
SPI_CS_TARGET     spi_cs_flash;
SPI               spi;//
struct SRAM_MAP   sram_map;

void testBuffer_1(){
    for(uint32_t i=0; i<SCREEN_BUFFER_SIZE; i++){
        uint32_t color_ = COLOR_BLUE;
        if(i%2 == 0) bufferWrite((color_ >> 16) & 0xff, (color_ >> 8) & 0xff, color_  & 0xff, i);
        else bufferWrite(0x00, 0x00, 0x00, i);
    }
}

void testBuffer_2(){
    for(uint32_t i=0; i<SCREEN_BUFFER_SIZE; i++){
        uint32_t color_ = COLOR_BLUE;
        if(i%2 != 0) bufferWrite((color_ >> 16) & 0xff, (color_ >> 8) & 0xff, color_  & 0xff, i);
        else bufferWrite(0x00, 0x00, 0x00, i);
    }
}

int main(void){

    /*________Initialize SRAM parameters______*/
    sramVarsInit(&spi);     

    /*________Initialize Protocols______*/
    spiInit(&spi);                             
    i2cInit(&i2c, true);                        
    uartInit();
    ws2812bInit();  
                                

    /*________Initialize Screen (Liquid Crystak)______*/
    screenInit(&screen, true);                             
    
    /*________Acquire SRAM access before operations______*/
    memAcquire();
    

    sramWriteModeRegister(&spi, SRAM_MODE_SEQU); 

    /*________Initialize Shared Memory System______*/
    sharedMemoryInit();

    
    memFree();

    while(1){
        /*___________Wait for Clear_To_Access signal__________*/
        memAcquire();

        /*________Control SPI data bus to access sram_________*/
        spiResume(&spi);

        getCommand();

        displayGrid();
        spiPause(&spi);
        memFree();
    }
}

void sramtesting(){
    sram_map.cmd.cmdID = WALK_LEFT;
    sram_map.cmd.arg1 = 0xAA;
    sram_map.cmd.arg2 = 0XBB;
    sram_map.cmd.arg3 = 0XCC;

    sram_map.score.current_score = 0X81;
    sram_map.score.record_score = 0X99;
    strcpy((char *)sram_map.score.game_name, "SNAKE");
    strcpy((char *)sram_map.score.player_name, "LUCAS");
    
    
    loadScore();
    loadCommand();
    sram_map.cmd.cmdID = 0;
    sram_map.cmd.arg1 = 0;
    sram_map.cmd.arg2 = 0;
    sram_map.cmd.arg3 = 0;

    sram_map.score.current_score = 0;
    sram_map.score.record_score = 0;
    memset(sram_map.score.game_name, 0, 12);
    memset(sram_map.score.player_name, 0, 12);
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
    cs_reg.CS_PIN = PB2;

    spi->conf = &spi_conf;
    spi->cs_reg = &cs_reg;
}