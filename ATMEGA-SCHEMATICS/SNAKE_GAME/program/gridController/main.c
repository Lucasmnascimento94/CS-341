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

<<<<<<< HEAD
    //char c[20];
    LED_DDR |= (1 << LED_PIN);   // data pin as output
    //DDRB |= (1<<PB1);
    //PORTB &= ~(1<<PB1);
    //seed_prng();
    gpioConfig();
    clear();


    //print("STARTING\n", 1);
   /* gameInit(&belly, &food);
    _delay_ms(500);
    initSnake(&belly, &food);
    belly.begin = true;
    push(&belly, PIXEL_ADDRESS(8, 5, 2), 8, 5, false);
    uint32_t addr = 0X00;

    uint16_t len_text = (uint16_t)(sizeof(c) - 1);
    uint16_t len_msg  = (uint16_t)(sizeof(msg)  - 1);
    uint16_t len_msg2 = (uint16_t)(sizeof(msg2) - 1);
    uint8_t buffer[300];         
    uint16_t len = sizeof(c);
    sramWriteModeRegister(SRAM_MODE_SEQU);
    sramReadModeRegister();
    sramWriteStringPoll(0x00, 0xFF, len);
    _delay_ms(10);
    */
    allocateVar();
    screenInit(&i2c_port, &i2c_target);


    char c[30];
    sprintf(c, "HELLO WORLD SNAKE PROJECT");
    uartWrite_(c);
    _delay_ms(100);
    memset(i2c_port.data, 0, strlen(i2c_port.data));
    i2c_port.data = c;
    screenWrite(&i2c_port, &i2c_target, &screen);
    //sprintf(c, "THIS IS ADDR IN CONF: %X\n", i2c_conf.slave[0]->addr);
   // uartWrite_(c);

    while(1){
        //test();
        //uartWrite("lOOPING\n", 9);
        screenWrite(&i2c_port, &i2c_target, &screen);
        _delay_ms(1000);
=======
    /*________Initialize Protocols______*/
    spiInit(&spi);                             
    i2cInit(&i2c, true);                        
    uartInit();
    ws2812bInit();                                 
    
    /*________Enforce SRAM Sequencial Mode______*/
    sramWriteModeRegister(&spi, SRAM_MODE_SEQU);

    /*________Initialize Screen (Liquid Crystak)______*/
    screenInit(&screen, true);                             

    /*________Initialize Shared Memory System______*/
    sharedMemoryInit();
    sramtesting();
    struct NODE food;
    loadFood(&food);


    bufferClear();
    displayClear();
    initialAnimation();
    initSnake();
    
    generateFood();
    displayGrid();

    while(1){
        while(!((PINC >> PC3) & 0x01)){uartWrite_("..waiting for ram..\n");}
        walk();
        displayGrid();
>>>>>>> c9ad762b83de567936d50458676836b7f9050e11
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
    cs_reg.CS_PIN = PB1;

    spi->conf = &spi_conf;
    spi->cs_reg = &cs_reg;
}