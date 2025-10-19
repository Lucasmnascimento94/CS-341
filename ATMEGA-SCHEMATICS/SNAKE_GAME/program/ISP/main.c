#include "start.h"
#include "spi.h"
#include "uart.h"
#include "avr/pgmspace.h"
#include "startSetUp/isp/isp.h"
#include "string.h"
//#include "./startSetUp/ws2812b/WS2812B.h"
//#include "snake/snake.h"
//#include "ws2812b/colors.h"
//#include "startSetUp/sram/sram.h"
//#include "i2c.h"
//#include "screen.h"
//#include "allocation.h"
#define F_CPU 16000000UL // CHANGE THIS to your AVR's actual clock speed
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


TARGET target = {
    .mc = 0,
    .status = 0xFF,
    .signature = 0x00,
};

PROGRAMMER  programmer = {
    .buffer_size = 0,
    .conf = NULL,
    .current_addr = 0x00,
    .current_page = 0x00,
    .page_counter = 0x00,
    .page_number = 0x00,
};

int main(void){
    char c[100] = {0};
    setUpUART();
    spiInitPoll();

    if(ispInit(&target)){
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

        ispChipErase();
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

    PORTB |= (1<<CS);
    while(1){
        //test();
        uartWrite_("lOOPING\n");
        _delay_ms(20000);
    }
}


/*

avrdude: AVR device initialized and ready to accept instructions
avrdude: device signature = 0x1e9406 (probably m168)
avrdude: reading lfuse memory ...
avrdude: writing output file <stdout>
0xe7
avrdude: reading hfuse memory ...
avrdude: writing output file <stdout>
0xd9

avrdude done.  Thank you.


*/