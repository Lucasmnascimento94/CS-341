#include "WS2812B.h"


static inline void DIN_H(void){ LED_PORT |=  (1<<LED_PIN); }
static inline void DIN_L(void){ LED_PORT &= ~(1<<LED_PIN); }
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


void playColor(){
    for(uint16_t i = 0; i<GRID_PIXELS; i++){send_pixel(COLOR_G, COLOR_G, COLOR_G);}
    latch();
    for(uint16_t i = 0; i<GRID_PIXELS; i++){send_pixel(0x00, 0x00, 0x00);}
    latch();
}

void cellbycell(){
    for(int i = 0; i<(int)GRID_PIXELS; i++){
        for(int j=0; j<(int)GRID_PIXELS; j++){
            if(j <= i){
                send_pixel(COLOR_G, COLOR_R, COLOR_B);
            }
            else{
                send_pixel(0x00, 0x00, 0x00);
            }
        }
        latch();
    }
}

void clear(){
    for(int i = 0; i<(int)GRID_PIXELS; i++){
        send_pixel(0x00, 0x00, 0x00);      
    }
    latch();
}

void snake(int size){
    for(int i = 0; i<(int)GRID_PIXELS; i++){
        for(int j=0; j<(int)GRID_PIXELS; j++){
            if(j <=i && j >= i-size){
                send_pixel(COLOR_G, COLOR_R, COLOR_B);
            }
            else{
                send_pixel(0x00, 0x00, 0x00);
            }
        }
        latch();
    }
}

/*
void loadGame(SnakeBelly *belly){
    //belly->count = SNAKE_SIZE_INIT;
}
void engGame(SnakeBelly *belly){

}
void startGame(SnakeBelly *belly){

}
void walk(SnakeBelly *belly){

}
*/