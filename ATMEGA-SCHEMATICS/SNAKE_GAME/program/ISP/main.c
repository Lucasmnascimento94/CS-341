#include "spi.h"
#include "uart.h"
#include "avr/pgmspace.h"
#include "isp.h"
#include "string.h"
#include "target.h"

//#include "./startSetUp/ws2812b/WS2812B.h"
//#include "snake/snake.h"
//#include "ws2812b/colors.h"
//#include "startSetUp/sram/sram.h"
//#include "i2c.h"
//#include "screen.h"
//#include "allocation.h"
#define F_CPU 8000000UL // CHANGE THIS to your AVR's actual clock speed


void sramVarsInit(TARGET_CONF *target);
void flashVarsInit(TARGET_CONF *target);
void gridVarsInit(TARGET_CONF *target);
void gameVarsInit(TARGET_CONF *target);
void programmerVarsInit();

const uint16_t page0[64] PROGMEM = {
0x940C, 0x0034, 0x940C, 0x003E, 0x940C, 0x003E, 0x940C, 0x003E, 
0x940C, 0x003E, 0x940C, 0x003E, 0x940C, 0x003E, 0x940C, 0x003E, 
0x940C, 0x003E, 0x940C, 0x003E, 0x940C, 0x003E, 0x940C, 0x003E, 
0x940C, 0x003E, 0x940C, 0x003E, 0x940C, 0x003E, 0x940C, 0x003E, 
0x940C, 0x003E, 0x940C, 0x003E, 0x940C, 0x003E, 0x940C, 0x003E, 
0x940C, 0x003E, 0x940C, 0x003E, 0x940C, 0x003E, 0x940C, 0x003E, 
0x940C, 0x003E, 0x940C, 0x003E, 0x2411, 0xBE1F, 0xEFCF, 0xE0D4, 
0xBFDE, 0xBFCD, 0x940E, 0x0040, 0x940C, 0x0066, 0x940C, 0x0000
};


const uint16_t page1[64] PROGMEM = {
0x9A20, 0x9A28, 0xB18A, 0x6C80, 0xB98A, 0xB18B, 0x6C80, 0xB98B, 
0xE021, 0xE490, 0xB185, 0x2782, 0xB985, 0xE8E7, 0xE1F3, 0x9731, 
0xF7F1, 0xC000, 0x0000, 0xB18B, 0x5880, 0xB98B, 0xE8E7, 0xE1F3, 
0x9731, 0xF7F1, 0xC000, 0x0000, 0xB18B, 0x2789, 0xB98B, 0xE8E7, 
0xE1F3, 0x9731, 0xF7F1, 0xC000, 0x0000, 0xCFE4, 0x94F8, 0xCFFF
};

/*SPI vars*/
SPI spi;

/*Targets*/
TARGET_CONF sram_conf;
TARGET_CONF flash_conf;
TARGET_CONF grid_conf;
TARGET_CONF game_conf;

ISP_TARGET sram;
ISP_TARGET flash;
ISP_TARGET grid;
ISP_TARGET game;

/*Isp*/
PROGRAMMER  programmer;


int main(void){

    char c[100] = {0};
    sramVarsInit(&sram_conf);
    flashVarsInit(&flash_conf);
    gridVarsInit(&grid_conf);
    gameVarsInit(&game_conf);
    programmerVarsInit();
    setUpUART();
    spiInit(&spi);

    DDRC |= (1<<PC1);
    PORTC |= (1<<PC1);
    targetSpiUpdate(&spi, &flash_conf);
    PORTD &= ~(1<<PD3);

    if(ispInit(&spi, &grid)){
        ispProgrammingEnable(&grid);
        ispReadSignatureByte(&grid, SIGNATURE_VENDOR);
        ispReadSignatureByte(&grid, SIGNATURE_FAMILY);
        ispReadSignatureByte(&grid, SIGNATURE_NUMBER);
        ispReadFuseBits(&grid, LFUSE);
        ispReadFuseBits(&grid, HFUSE);
        ispReadFuseBits(&grid, EXTFUSE);

        grid.signature = \
            ((uint32_t)grid.signature_vendor << 16) | \
            ((uint16_t)grid.signature_family << 8)  | \
            (grid.signature_number);
        
        sprintf(c, "Device Vendor: %X\n", (unsigned)grid.signature_vendor);
        uartWrite_(c);
        sprintf(c, "Device Family: %X\n", (unsigned)grid.signature_family);
        uartWrite_(c);
        sprintf(c, "Device Number: %X\n", (unsigned)grid.signature_number);
        uartWrite_(c);
        sprintf(c, "Device Signature: %lX\n", (unsigned long)grid.signature);
        uartWrite_(c);

        sprintf(c, "Device lfuse: %lX\n", (unsigned long)grid.lfuse);
        uartWrite_(c);
        sprintf(c, "Device hfuse: %lX\n", (unsigned long)grid.hfuse);
        uartWrite_(c);
        sprintf(c, "Device extfuse: %lX\n", (unsigned long)grid.exfuse);
        uartWrite_(c);

        _delay_ms(100);

        ispChipErase(&spi);
        uartWrite_("ERASED PAGE\n");
        _delay_ms(20);
        programmer.buffer_size = sizeof(page0)/sizeof(page0[0]);
        sprintf(c, "this is size_0: %d\n", programmer.buffer_size);
        uartWrite_(c);
        programmer.buffer = page0;
        ispLoadProgramMemoryPage(&programmer);

        _delay_ms(100);
        ispVerifyProgramMemoryPage(&programmer, programmer.current_page);

        _delay_ms(300);
        programmer.buffer_size = sizeof(page1)/sizeof(page1[0]);
        sprintf(c, "this is size_1: %d\n", programmer.buffer_size);
        uartWrite_(c);
        programmer.buffer = page1;
        ispLoadProgramMemoryPage(&programmer);
        _delay_ms(100);
        ispVerifyProgramMemoryPage(&programmer, programmer.current_page);

    }
    else{
        uartWrite_("Failed to Sync\n");
    }

    *spi.cs_reg->CS_PORT |= (1<<spi.cs_reg->CS_PIN);
    

    while(1){
        //test();
        uartWrite_("lOOPING\n");
        _delay_ms(20000);
    }
}

void sramVarsInit(TARGET_CONF *target){
    static SPI_CS_TARGET cs_reg;
    static TARGET_SIGNAL signal;
    static SPI_CONF spi_conf;
    static SPI_MODE spi_mode;

    spi_mode.en = true;
    spi_mode.irq = false;
    spi_mode.mode = 0;
    spi_mode.lsbfirst = false;
    spi_mode.prescaler = 64;
    spi_mode.mstr = true;
    spi_conf.mode_conf = &spi_mode;

    cs_reg.CS_DDR = &SRAM_CS_DDR;
    cs_reg.CS_PORT = &SRAM_CS_PORT;
    cs_reg.CS_PIN = SRAM_CS_PIN;

    signal.SIGNAL_DDR = &SRAM_DDR;
    signal.SIGNAL_PORT = &SRAM_PORT;
    signal.SIGNAL_PIN = SRAM_PIN;

    target->cs_reg = &cs_reg;
    target->signal = &signal;
    target->spi_conf = &spi_conf;
}

void flashVarsInit(TARGET_CONF *target){
    static SPI_CS_TARGET cs_reg;
    static TARGET_SIGNAL signal;
    static SPI_CONF spi_conf;
    static SPI_MODE spi_mode;

    spi_mode.en = true;
    spi_mode.irq = false;
    spi_mode.mode = 0;
    spi_mode.lsbfirst = false;
    spi_mode.prescaler = 64;
    spi_mode.mstr = true;
    spi_conf.mode_conf = &spi_mode;

    cs_reg.CS_DDR = &FLASH_CS_DDR;
    cs_reg.CS_PORT = &FLASH_CS_PORT;
    cs_reg.CS_PIN = FLASH_CS_PIN;

    signal.SIGNAL_DDR = &FLASH_DDR;
    signal.SIGNAL_PORT = &FLASH_PORT;
    signal.SIGNAL_PIN = FLASH_PIN; 
    
    target->cs_reg = &cs_reg;
    target->signal = &signal;
    target->spi_conf = &spi_conf;
}

void gridVarsInit(TARGET_CONF *target){
    static SPI_CS_TARGET cs_reg;
    static TARGET_SIGNAL signal;
    static SPI_CONF spi_conf;
    static SPI_MODE spi_mode;

    spi_mode.en = true;
    spi_mode.irq = false;
    spi_mode.mode = 0;
    spi_mode.lsbfirst = false;
    spi_mode.prescaler = 64;
    spi_mode.mstr = true;
    spi_conf.mode_conf = &spi_mode;

    cs_reg.CS_DDR = &GRID_CS_DDR;
    cs_reg.CS_PORT = &GRID_CS_PORT;
    cs_reg.CS_PIN = GRID_CS_PIN;

    signal.SIGNAL_DDR = &GRID_DDR;
    signal.SIGNAL_PORT = &GRID_PORT;
    signal.SIGNAL_PIN = GRID_PIN;  
    
    target->cs_reg = &cs_reg;
    target->signal = &signal;
    target->spi_conf = &spi_conf;
}

void gameVarsInit(TARGET_CONF *target){
    static SPI_CS_TARGET cs_reg;
    static TARGET_SIGNAL signal;
    static SPI_CONF spi_conf;
    static SPI_MODE spi_mode;

    spi_mode.en = true;
    spi_mode.irq = false;
    spi_mode.mode = 0;
    spi_mode.lsbfirst = false;
    spi_mode.prescaler = 64;
    spi_mode.mstr = true;
    spi_conf.mode_conf = &spi_mode;

    cs_reg.CS_DDR = &GAME_CS_DDR;
    cs_reg.CS_PORT = &GAME_CS_PORT;
    cs_reg.CS_PIN = GAME_CS_PIN;

    signal.SIGNAL_DDR = &GAME_DDR;
    signal.SIGNAL_PORT = &GAME_PORT;
    signal.SIGNAL_PIN = GAME_PIN; 
    
    target->cs_reg = &cs_reg;
    target->signal = &signal;
    target->spi_conf = &spi_conf;
}

void programmerVarsInit(){
    grid.mc = 0;
    grid.status = 0xFF;
    grid.signature = 0x00;

    programmer.buffer_size = 0;
    programmer.buffer_size = 0;
    programmer.conf = NULL;
    programmer.current_addr = 0x00;
    programmer.current_page = 0x00;
    programmer.page_counter = 0x00;
    programmer.page_number = 0x00;
}







        /*ispProgrammingEnable(&grid);
        ispReadSignatureByte(&grid, SIGNATURE_VENDOR);
        ispReadSignatureByte(&grid, SIGNATURE_FAMILY);
        ispReadSignatureByte(&grid, SIGNATURE_NUMBER);
        ispReadFuseBits(&grid, LFUSE);
        ispReadFuseBits(&grid, HFUSE);
        ispReadFuseBits(&grid, EXTFUSE);

        grid.signature = \
            ((uint32_t)grid.signature_vendor << 16) | \
            ((uint16_t)grid.signature_family << 8)  | \
            (grid.signature_number);
        
        sprintf(c, "Device Vendor: %X\n", (unsigned)grid.signature_vendor);
        uartWrite_(c);
        sprintf(c, "Device Family: %X\n", (unsigned)grid.signature_family);
        uartWrite_(c);
        sprintf(c, "Device Number: %X\n", (unsigned)grid.signature_number);
        uartWrite_(c);
        sprintf(c, "Device Signature: %lX\n", (unsigned long)grid.signature);
        uartWrite_(c);

        sprintf(c, "Device lfuse: %lX\n", (unsigned long)grid.lfuse);
        uartWrite_(c);
        sprintf(c, "Device hfuse: %lX\n", (unsigned long)grid.hfuse);
        uartWrite_(c);
        sprintf(c, "Device extfuse: %lX\n", (unsigned long)grid.exfuse);
        uartWrite_(c);

        _delay_ms(100);

        ispChipErase(&spi);
        uartWrite_("ERASED PAGE\n");
        _delay_ms(20);
        programmer.buffer_size = sizeof(page0)/sizeof(page0[0]);
        sprintf(c, "this is size_0: %d\n", programmer.buffer_size);
        uartWrite_(c);
        programmer.buffer = page0;
        ispLoadProgramMemoryPage(&programmer);

        _delay_ms(100);
        ispVerifyProgramMemoryPage(&programmer, programmer.current_page);

        _delay_ms(300);
        programmer.buffer_size = sizeof(page1)/sizeof(page1[0]);
        sprintf(c, "this is size_1: %d\n", programmer.buffer_size);
        uartWrite_(c);
        programmer.buffer = page1;
        ispLoadProgramMemoryPage(&programmer);
        _delay_ms(100);
        ispVerifyProgramMemoryPage(&programmer, programmer.current_page);

    }
    else{
        uartWrite_("Failed to Sync\n");
    }

    *spi.cs_reg->CS_PORT |= (1<<spi.cs_reg->CS_PIN);*/