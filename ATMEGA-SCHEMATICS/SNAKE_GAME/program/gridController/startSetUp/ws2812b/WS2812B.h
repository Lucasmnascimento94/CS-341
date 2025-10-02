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
    #define TLL 100
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
#define  COLOR_R 0x00
#define  COLOR_B 0xFF


#define PAD_WIDTH   8
#define STRIDE  256
#define PIXEL_ADDRESS(i,j,k) \
  ( ((j)%2)==0 ? (((j)-1)*(PAD_WIDTH) + ((i)) + ((k)-1)*(STRIDE)) \
               : (((j))*(PAD_WIDTH) + ((1)-(i))  + ((k)-1)*(STRIDE)) )
#define CURRENT_PAD(i) ((((i)-1)/PAD_WIDTH) + 1)
#define LOCAL_I(i, k) ((i) - (((k) - 1)*PAD_WIDTH))
#define FIRST_PIXEL_X 24
#define FIRST_PIXEL_Y 16
#define SNAKE_SIZE_INIT 20
#define SNAKE_MAX_COUNT 40

void clear();
void displayGrid(SnakeBelly *belly, struct Cell *food);


#endif