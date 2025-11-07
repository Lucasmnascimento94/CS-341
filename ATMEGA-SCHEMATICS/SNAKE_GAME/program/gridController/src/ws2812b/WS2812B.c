#include "WS2812B.h"
#include "uart.h"
#include "string.h"
#include "shared_memory.h" // holds the buffer

static inline void DIN_H(void){ DATA_PORT |=  (1<<DATA_PIN); }
static inline void DIN_L(void){ DATA_PORT &= ~(1<<DATA_PIN); }
static inline void dcy(uint16_t c){__builtin_avr_delay_cycles(c);}


static inline void writeZero(void){ DIN_H(); dcy(T0H);  DIN_L(); dcy(T0L); }
static inline void writeOne(void){ DIN_H(); dcy(T1H);  DIN_L(); dcy(T1L);  }
static inline void latch(void){DIN_L(); dcy(TLL);}

static inline void send_byte(uint8_t b){
    for (uint8_t m = 0x80; m; m >>= 1) (b & m) ? writeOne() : writeZero();
}

static inline void send_pixel(uint8_t g, uint8_t r, uint8_t b){ // GRB order
    send_byte(g); send_byte(r); send_byte(b);
}

void displayGrid(){
   // insertion_sort(pixels, belly->count);
    for(uint16_t i = 0; i< SCREEN_BUFFER_SIZE; i++){
        uint32_t data = 0x00;
        bufferRead(&data, i);
        send_pixel((data>> 16) & 0xFF, (data >> 8) & 0xFF, (data) & 0xFF);
    }
/*
    for(uint16_t i = 1; i< SCREEN_BUFFER_SIZE; i++){

            if(k < 100 && (pixels[k] == i)){
                send_pixel((belly->color >> 16) & 0xFF, (belly->color >> 8) & 0xFF, (belly->color) & 0xFF);
                k++;
            }

            else if(belly->begin && i == food->val){
                send_pixel((belly->color_food >> 16) & 0xFF, (belly->color_food >> 8) & 0xFF, (belly->color_food) & 0xFF);
            }
            else{
                send_pixel(0x00, 0x00, 0x00);
            }
    }
*/
    latch();
}

void clear(){
    for(int i = 0; i<(int)SCREEN_BUFFER_SIZE; i++){
        send_pixel(0x00, 0x00, 0x00);      
    }
    latch();
}
