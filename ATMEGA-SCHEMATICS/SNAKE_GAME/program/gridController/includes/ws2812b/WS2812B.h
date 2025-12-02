#ifndef WS2812B_H
#define WS2812B_H
#include "includes.h"
#include "start.h"
#include "DataStructures.h"



#define IO_WRITE_CYC 4


#ifndef CYCLE_NS
  #if !defined(F_CPU)
    #error "F_CPU must be defined before including this header"
  #endif

  #if   (F_CPU == 16000000UL)
    #define T1H_TOT 13  
    #define T1L_TOT 7    
    #define T0H_TOT 6    
    #define T0L_TOT 14   
    #define TLL 15
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

#define COLOR_G 0xFF
#define COLOR_R 0x00
#define COLOR_B 0xFF

#define DATA_PORT_ PORTD
#define DATA_DDR_ DDRD
#define DATA_PIN_1 PD0
#define DATA_PIN_2 PD1
#define DATA_PIN_3 PD2
#define DATA_PIN_4 PD3
#define DATA_PIN_5 PD4
#define DATA_PIN_6 PD5
#define DATA_PIN_7 PD6
#define DATA_PIN_8 PD7


#define DATA_PORT PORTD
#define DATA_PIN PD2
#define DATA_DDR DDRD



#define PAD_WIDTH  8
#define PAD_HEIGHT 8
#define PAD_LEN PAD_HEIGHT*PAD_WIDTH
#define STRIDE  256
#define PIXEL_ADDRESS(i,j) \
  ( ((j)%2)==0 ?\
                  (((PAD_WIDTH-1) - (i%PAD_WIDTH)) + (PAD_WIDTH*j) + (((i/PAD_WIDTH)*(STRIDE)))):\
                  ((PAD_WIDTH*j) + (i%PAD_WIDTH) + (((i/PAD_WIDTH)*(STRIDE)))))

#define FIRST_PIXEL_X 4
#define FIRST_PIXEL_Y 4
#define SNAKE_SIZE_INIT 10
#define SNAKE_MAX_COUNT 1000

void send_byte(uint8_t b);
void writeZero(void);
void latch(void);
void writeOne(void);
void displayClear();
void displayGrid();
void ws2812bInit();
void ws2812bWrite(uint8_t g, uint8_t r, uint8_t b, uint8_t i, uint8_t j);
uint32_t ws2812bGetAddress(uint8_t i, uint8_t j);
void testWs2812b();

#endif
