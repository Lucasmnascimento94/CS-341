#include "start.h"
#include "spi.h"
#include "uart.h"
#include "avr/pgmspace.h"
#include "../ISP/isp.h"
#include "string.h"
//#include "./startSetUp/ws2812b/WS2812B.h"
//#include "snake/snake.h"
//#include "ws2812b/colors.h"
//#include "startSetUp/sram/sram.h"
//#include "i2c.h"
//#include "screen.h"
//#include "allocation.h"

const uint16_t page0[64] PROGMEM = {
0x0C94, 0x3400, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 
0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 
0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 
0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 
0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 
0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x0C94, 0x3E00, 
0x0C94, 0x3E00, 0x0C94, 0x3E00, 0x1124, 0x1FBE, 0xCFEF, 0xD4E0, 
0xDEBF, 0xCDBF, 0x0E94, 0x4000, 0x0C94, 0x6600, 0x0C94, 0x0000
};


const uint16_t page1[64] PROGMEM = {
0x209A, 0x289A, 0x8AB1, 0x806C, 0x8AB9, 0x8BB1, 0x806C, 0x8BB9, 
0x21E0, 0x90E4, 0x85B1, 0x8227, 0x85B9, 0xE7E8, 0xF3E1, 0x3197, 
0xF1F7, 0x00C0, 0x0000, 0x8BB1, 0x8058, 0x8BB9, 0xE7E8, 0xF3E1, 
0x3197, 0xF1F7, 0x00C0, 0x0000, 0x8BB1, 0x8927, 0x8BB9, 0xE7E8, 
0xF3E1, 0x3197, 0xF1F7, 0x00C0, 0x0000, 0xE4CF, 0xF894, 0xFFCF
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