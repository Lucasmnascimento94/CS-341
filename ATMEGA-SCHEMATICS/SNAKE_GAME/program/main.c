#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdlib.h>

#define LED_PORT PORTC
#define LED_DDR  DDRC
#define LED_PIN  PC0
#define GRID_PIXELS (32u*32u)
#define IO_WRITE_CYC 6

/*
    PROTOCOL TIMING CONSTRAINS

    #writeOne -> T1H (0.8us = 800ns) + T1L (0.45us = 450ns) >> 1250ns = 1.25us 
    #writeZero -> T0H (0.4us = 400ns) + T0L (0.85us = 850ns) >> 1250ns = 1.25us
    Reset -> T0L (50us).
    #Delay -> 50us.
*/


/*
    16MHz Crystal
    1 cy -> 1/16000000 -> 63ns

    800ns -> 13*cy = 819ns
    450ns -> 7*cy  = 441 ns
    ###-> WriteOne -> T1H(13*cy) + T1L(7*cy) -> T = 1260ns


    850s -> 14*cy = 882ns
    400ns -> 6*cy  = 378 ns
    ###-> WriteOne -> T0H(6*cy) + T0L(14*cy) -> T = 1260ns
*/

/*
    20MHz Crystal
    1 cy -> 1/20000000 -> 50 ns

    700ns -> 16*cy = 800 ns
    550ns -> 9*cy = 450 ns
    ###-> WriteOne  -> T1H(16*cy) + T1L(9*cy) -> T = 1250 ns

    400ns ->  8*cy = 400 ns
    850ns -> 17*cy = 850 ns
    ###-> WriteZero -> T0H(8*cy)  + T0L(17*cy) -> T = 1250 ns
*/


/* WS2812 @16 MHz (approx target):
   0-bit: H≈0.35µs (~6 cyc), L≈0.80µs (~13 cyc)
   1-bit: H≈0.70µs (~11 cyc), L≈0.60µs (~10 cyc)
   Account ~2 cyc for each port write.
*/

#ifndef CYCLE_NS
  #if !defined(F_CPU)
    #error "F_CPU must be defined before including this header"
  #endif

  #if   (F_CPU == 16000000UL)
    #define T1H_TOT 13  
    #define T1L_TOT 7    
    #define T0H_TOT 6    
    #define T0L_TOT 13   
    #define TLL 10000
  #elif (F_CPU == 20000000UL)
    #define T1H_TOT 14   // 0.70us high
    #define T1L_TOT 9   // 0.55us low
    #define T0H_TOT 7    // 0.35us high
    #define T0L_TOT 18   // 0.90us low
    #define TLL 100
  #else
    #define CYCLE_NS (1000000000UL / (F_CPU)) /* generic fallback */
  #endif
#endif

#define T1H (T1H_TOT - IO_WRITE_CYC)
#define T1L (T1L_TOT - IO_WRITE_CYC)
#define T0H (T0H_TOT - IO_WRITE_CYC)
#define T0L (T0L_TOT - IO_WRITE_CYC)


void seed_prng(void){ srand(0xA5A5 ^ (uint16_t)TCNT0); }
static inline void DIN_H(void){ LED_PORT |=  (1<<LED_PIN); }
static inline void DIN_L(void){ LED_PORT &= ~(1<<LED_PIN); }
static inline void dcy(uint16_t c){ __builtin_avr_delay_cycles(c); }


static inline void writeZero(void){ DIN_H(); dcy(T0H);  DIN_L(); dcy(T0L); }
static inline void writeOne(void){ DIN_H(); dcy(T1H);  DIN_L(); dcy(T1L);  }
static inline void latch(void){DIN_L(); dcy(TLL);}

static inline void send_byte(uint8_t b){
    for (uint8_t m = 0x80; m; m >>= 1) (b & m) ? writeOne() : writeZero();
}

static inline void send_pixel(uint8_t g, uint8_t r, uint8_t b){ // GRB order
    send_byte(g); send_byte(r); send_byte(b);
}

uint8_t COLOR_G = 0xFF;
uint8_t COLOR_R = 0x00;
uint8_t COLOR_B = 0xFF;


void playColor(){
    int r_g = rand(); int r_r = rand(); int r_b = rand();
    uint8_t g = (uint8_t)r_g; uint8_t r = (uint8_t)r_r; uint8_t b = (uint8_t)r_b; 
    for(int i = 0; i<GRID_PIXELS; i++){send_pixel(COLOR_G, COLOR_G, COLOR_G);}
    latch();
    for(int i = 0; i<GRID_PIXELS; i++){send_pixel(0x00, 0x00, 0x00);}
    latch();
}

void cellbycell(){
    for(int i = 0; i<GRID_PIXELS; i++){
        for(int j=0; j<GRID_PIXELS; j++){
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
    for(int i = 0; i<GRID_PIXELS; i++){
        send_pixel(0x00, 0x00, 0x00);      
    }
    latch();
}

void snake(uint8_t size){
    for(int i = 0; i<GRID_PIXELS; i++){
        for(int j=0; j<GRID_PIXELS; j++){
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


int main(void){
    LED_DDR |= (1 << LED_PIN);   // data pin as output
    seed_prng();
    while(1){
        //playColor();
        clear();
        _delay_ms(500);
        snake(15);
       // clear();
        _delay_ms(500);
    }
    
}
