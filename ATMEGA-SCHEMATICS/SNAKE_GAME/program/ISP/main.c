#include "start.h"
#include "spi.h"
#include "uart.h"
#include "avr/pgmspace.h"
#include "isp.h"
#include "string.h"
//#include "./startSetUp/ws2812b/WS2812B.h"
//#include "snake/snake.h"
//#include "ws2812b/colors.h"
//#include "startSetUp/sram/sram.h"
//#include "i2c.h"
//#include "screen.h"
//#include "allocation.h"
#define F_CPU 8000000UL // CHANGE THIS to your AVR's actual clock speed
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
void initVars();

/*SPI vars*/
SPI spi;
SPI_CONF spi_conf;
SPI_CS_TARGET spi_cs;
SPI_MODE spi_mode;

/*ISP vars*/
TARGET target;
PROGRAMMER  programmer;


int main(void){
    char c[100] = {0};
    initVars();
    setUpUART();
    spiInit(&spi);

    if(ispInit(&spi, &target)){
        ispProgrammingEnable(&target);
        ispReadSignatureByte(&target, SIGNATURE_VENDOR);
        ispReadSignatureByte(&target, SIGNATURE_FAMILY);
        ispReadSignatureByte(&target, SIGNATURE_NUMBER);
        ispReadFuseBits(&target, LFUSE);
        ispReadFuseBits(&target, HFUSE);
        ispReadFuseBits(&target, EXTFUSE);

        target.signature = \
            ((uint32_t)target.signature_vendor << 16) | \
            ((uint16_t)target.signature_family << 8)  | \
            (target.signature_number);
        
        sprintf(c, "Device Vendor: %X\n", (unsigned)target.signature_vendor);
        uartWrite_(c);
        sprintf(c, "Device Family: %X\n", (unsigned)target.signature_family);
        uartWrite_(c);
        sprintf(c, "Device Number: %X\n", (unsigned)target.signature_number);
        uartWrite_(c);
        sprintf(c, "Device Signature: %lX\n", (unsigned long)target.signature);
        uartWrite_(c);

        sprintf(c, "Device lfuse: %lX\n", (unsigned long)target.lfuse);
        uartWrite_(c);
        sprintf(c, "Device hfuse: %lX\n", (unsigned long)target.hfuse);
        uartWrite_(c);
        sprintf(c, "Device extfuse: %lX\n", (unsigned long)target.exfuse);
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

    *spi.reg->CS_PORT |= (1<<spi.reg->CS_PIN);
    while(1){
        //test();
        uartWrite_("lOOPING\n");
        _delay_ms(20000);
    }
}


void initVars(){
    target.mc = 0;
    target.status = 0xFF;
    target.signature = 0x00;

    programmer.buffer_size = 0;
    programmer.buffer_size = 0;
    programmer.conf = NULL;
    programmer.current_addr = 0x00;
    programmer.current_page = 0x00;
    programmer.page_counter = 0x00;
    programmer.page_number = 0x00;

    spi.conf = &spi_conf;
    spi.reg = &spi_cs;
    spi.conf->mode_conf = &spi_mode;

    spi_mode.en = true;
    spi_mode.irq = false;
    spi_mode.mode = 0;
    spi_mode.lsbfirst = false;
    spi_mode.prescaler = 64;
    spi_mode.mstr = true;

    spi_cs.CS_DDR = &DDRC;
    spi_cs.CS_PORT = &PORTC;
    spi_cs.CS_PIN = PC0;
}
