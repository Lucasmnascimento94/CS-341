#include "includes.h"
#include "spi.h"
#include "uart.h"
#include "sram.h"
#include "shared_memory.h"
#include "console.h"
#include "isr.h"


void sramInitial();
void init();
void sramVarsInit(SPI *spi);

SPI_CS_TARGET     spi_cs_flash;
SPI               spi;
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
    char c[50];
    enableReg();
    seed_prng();
    init();
    isr_flag = sram_map.cmd.cmdID;
    while(1){
        /*________Control SPI data bus to access SRAM to write commands only_________*/
        memAcquire();
        spiResume(&spi);

        if(isr_flag == WALK_UP)      sram_map.cmd.cmdID = WALK_UP;
        else if(isr_flag == WALK_DOWN)  sram_map.cmd.cmdID = WALK_DOWN;
        else if(isr_flag == WALK_LEFT)  sram_map.cmd.cmdID = WALK_LEFT;
        else                            sram_map.cmd.cmdID = WALK_RIGHT;

        loadCommand();   /* write latest joystick command into shared SRAM */

        spiPause(&spi);
        memFree();
        _delay_us(10);
    }
}

void init(){
    consoleConf();
    
    /*________Initialize SRAM parameters______*/
    sramVarsInit(&spi);     

    /*________Initialize Protocols______*/
    spiInit(&spi);      
    uartInit();
    uartWrite_("hello from game\n");                                                            
    memAcquire();

    /*________Enforce SRAM Sequencial Mode______*/
    sramWriteModeRegister(&spi, SRAM_MODE_SEQU);                         

    /*________Initialize Shared Memory System______*/
    sharedMemoryInit();
    sramInitial();

    bufferClear();
    initSnake();
    generateFood();

    loadScore();
    loadCommand();
    loadStack();
    loadBufferFromStack();
    memFree();
    _delay_ms(3000);
}

void sramInitial(){
    sram_map.cmd.cmdID = WALK_RIGHT;
    sram_map.cmd.arg1 = 0x00;
    sram_map.cmd.arg2 = 0X00;
    sram_map.cmd.arg3 = 0X00;

    sram_map.score.current_score = 0X00;
    sram_map.score.record_score = 0X00;
    strcpy((char *)sram_map.score.game_name, "SNAKE");
    strcpy((char *)sram_map.score.player_name, "LUCAS");
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

    cs_reg.CS_DDR = &DDRC;
    cs_reg.CS_PORT = &PORTC;
    cs_reg.CS_PIN = PC1;

    spi->conf = &spi_conf;
    spi->cs_reg = &cs_reg;
}