#include "includes.h"
#include "spi.h"
#include "uart.h"
#include "i2c.h"
#include "sram.h"
#include "screen.h"
#include "shared_memory.h"
#include "isr.h"

#define CS_RAM_PORT GPIOB
#define CS_RAM_PIN PB1

#define CS_STM32_PORT GPIOB
#define CS_STM32_PIN PB2


void sramtesting();
void sramVarsInit(SPI *spi);

SCREEN            screen;
I2C_CONF          i2c;
SPI_CS_TARGET     spi_cs_flash;
SPI               spi;
struct SRAM_MAP   sram_map;


int main(void){
    /*________Initialize SRAM parameters______*/
    sramVarsInit(&spi);     

    /*________Initialize Protocols______*/
    uartInit(); 
    i2cInit(&i2c, true);   
    spiInit(&spi);
    ISR_SPI_CONF((char *)spi.buffer);                                                    
    
    /*________Enforce SRAM Sequencial Mode______*/
    //sramWriteModeRegister(&spi, SRAM_MODE_SEQU);

    /*________Initialize Screen (Liquid Crystak)______*/
    screenInit(&screen, true);                             

    /*________Initialize Shared Memory System______*/
    //sharedMemoryInit();
    sramtesting();
    //struct NODE food;
    //loadFood(&food);

    screenWrite(&screen, "hello from snake in the basement");
    while(1){
        uartWrite_("looping\n");
        _delay_ms(1000);
    }
}

void sramtesting(){
    sram_map.cmd.arg1 = 0xAA;
    sram_map.cmd.arg2 = 0XBB;
    sram_map.cmd.arg3 = 0XCC;

    sram_map.score.current_score = 0X81;
    sram_map.score.record_score = 0X99;
    strcpy((char *)sram_map.score.game_name, "SNAKE GAME");
    strcpy((char *)sram_map.score.player_name, "LUCAS NASC");
    
    loadScore();
    //loadCommand();
    sram_map.cmd.cmdID = 0;
    sram_map.cmd.arg1 = 0;
    sram_map.cmd.arg2 = 0;
    sram_map.cmd.arg3 = 0;

    sram_map.score.current_score = 0;
    sram_map.score.record_score = 0;
    memset(sram_map.score.game_name, 0, 12);
    memset(sram_map.score.player_name, 0, 12);

    getCommand();
    getScore();
    uartWrite_("GAME NAME: ");
    _delay_us(200);
    uartWrite_((char *)sram_map.score.game_name);
    _delay_us(200);
    uartWrite_("\nPLAYER NAME: ");
    _delay_us(200);
    uartWrite_((char *)sram_map.score.player_name);
    uartWrite_("\n");
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
    spi_mode.mstr = SPI_MASTER_SLAVE;
    spi_conf.mode_conf = &spi_mode;

    cs_reg.CS_DDR = &DDRB;
    cs_reg.CS_PORT = &PORTB;
    cs_reg.CS_PIN = PB1;

    spi->conf = &spi_conf;
    spi->cs_reg = &cs_reg;
}