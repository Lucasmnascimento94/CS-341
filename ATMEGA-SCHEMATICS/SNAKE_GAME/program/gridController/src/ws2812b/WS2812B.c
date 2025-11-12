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

void writeBurst(uint8_t *buff1){
    cli();
        for(uint16_t j=0; j< PAD_LEN*3*8; j++){

            /*First 0.4us cycle*/
            uint8_t burst_ = 0x00 | (1<<DATA_PIN_1) | (1<<DATA_PIN_2) | (1<<DATA_PIN_3) | (1<<DATA_PIN_4) | (1<<DATA_PIN_5) | (1<<DATA_PIN_6) | (1<<DATA_PIN_7) | (1<<DATA_PIN_8);
            DATA_PORT_ |= burst_;
            dcy(4);
            
            /*Second 0.4us cycle*/
            DATA_PORT_ &= buff1[j];
            dcy(6);
            /*Third 0.4us cycle*/
            DATA_PORT_ &= ~(1<<DATA_PIN_1) & ~(1<<DATA_PIN_2) & ~(1<<DATA_PIN_3) & ~(1<<DATA_PIN_4) & ~(1<<DATA_PIN_5) & ~(1<<DATA_PIN_6) & ~(1<<DATA_PIN_7) & ~(1<<DATA_PIN_8);
            dcy(4);
        }
                    
    sei();

    latch();
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
    for(uint32_t i = 0; i<(int)SCREEN_BUFFER_SIZE; i++){
        bufferWrite(0, 0, 0, i);   
    }
    latch();
}

void ws2812bInit(){
    
    DATA_DDR_ |= (1<<DATA_PIN_1) | (1<<DATA_PIN_2) | (1<<DATA_PIN_3) | (1<<DATA_PIN_4) | (1<<DATA_PIN_5) | (1<<DATA_PIN_6) | (1<<DATA_PIN_7) | (1<<DATA_PIN_8);
    DATA_PORT_ |= (1<<DATA_PIN_1) | (1<<DATA_PIN_2) | (1<<DATA_PIN_3) | (1<<DATA_PIN_4) | (1<<DATA_PIN_5) | (1<<DATA_PIN_6) | (1<<DATA_PIN_7) | (1<<DATA_PIN_8);
}

void ws2812bWrite(uint8_t g, uint8_t r, uint8_t b, uint8_t i, uint8_t j){
    bufferWrite(g, r, b, PIXEL_ADDRESS(i,j));
}

void testWs2812b(){
    for(int j=0; j<SCREEN_HEIGHT; j++){
        for(int i=0; i<SCREEN_WIDTH; i++){
            bufferWrite(0xff, 0x00, 0x00, PIXEL_ADDRESS(i, j));
            displayGrid();
            bufferWrite(0,0,0,PIXEL_ADDRESS(i, j));
        }
    }
}

#if (GAME == SNAKE)

#elif (GAME == SPACE)

#elif (GAME == PONG)

#endif