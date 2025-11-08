#include "WS2812B.h"
#include "uart.h"
#include "string.h"
#include "shared_memory.h" // holds the buffer

static inline void DIN_H(void){ DATA_PORT |=  (1<<DATA_PIN); }
static inline void DIN_L(void){ DATA_PORT &= ~(1<<DATA_PIN); }
static inline void dcy(uint16_t c){__builtin_avr_delay_cycles(c);}


void writeZero(void){ DIN_H(); if(T0H>0)dcy(T0H);  DIN_L(); if(T0L>0)dcy(T0L); }
void writeOne(void){ DIN_H(); if(T1H>0)dcy(T1H);  DIN_L(); if(T1L>0)dcy(T1L);  }
void latch(void){DIN_L(); dcy(TLL);}

void send_byte(uint8_t b){
    for (uint8_t m = 0x80; m; m >>= 1) (b & m) ? writeOne() : writeZero();
}

static inline void send_pixel(uint8_t g, uint8_t r, uint8_t b){ // GRB order
    send_byte(g); send_byte(r); send_byte(b);
}

void displayGrid(){
   // insertion_sort(pixels, belly->count);
    bufferRead();
    latch();
}

void displayClear(){
    for(int i = 0; i<(int)SCREEN_BUFFER_SIZE; i++){
        send_pixel(0x00, 0x00, 0x00);      
    }
    latch();
}

void ws2812bInit(){
    DATA_DDR |= (1<<DATA_PIN);
    DATA_PORT |= (1<<DATA_PIN);
}

void ws2812bWrite(uint8_t g, uint8_t r, uint8_t b, uint8_t i, uint8_t j){
    
}
#if (GAME == SNAKE)

#elif (GAME == SPACE)

#elif (GAME == PONG)

#endif