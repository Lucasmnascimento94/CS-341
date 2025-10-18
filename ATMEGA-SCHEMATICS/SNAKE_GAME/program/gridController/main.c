#include "start.h"
#include "spi.h"
#include "uart.h"
#include "../ISP/isp.h"
#include "string.h"
//#include "./startSetUp/ws2812b/WS2812B.h"
//#include "snake/snake.h"
//#include "ws2812b/colors.h"
//#include "startSetUp/sram/sram.h"
//#include "i2c.h"
//#include "screen.h"
//#include "allocation.h"
TARGET target = {
    .mc = 0,
    .status = 0xFF,
    .signature = 0x00,
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
    }
    else{
        uartWrite_("Failed to Sync\n");
    }
    while(1){
        //test();
        uartWrite_("lOOPING\n");
        _delay_ms(5000);
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