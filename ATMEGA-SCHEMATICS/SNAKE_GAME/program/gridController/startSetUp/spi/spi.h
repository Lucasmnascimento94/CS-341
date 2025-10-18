#ifndef SPI_H
#define SPI_H

#include "includes.h"


/*==============================================================================
 *  UART MODULE CONFIGURATION
 *==============================================================================
 *  Reference:
 *    ---
 *
 *  Description:
 *    ---
 *
 *============================================================================*/
/*==============================================================================
 *  END OF UART MODULE CONFIGURATION
*==============================================================================*/






/*==============================================================================
 *  SPI MODULE CONFIGURATION
 *==============================================================================
 *  Reference:
 *    - SPI Application Note : ./documentation/protocols/SPI.md
 *    - SRAM Application Note: ./documentation/grid/SRAM.md
 *
 *  Description:
 *    This section contains configuration and setup for the SPI peripheral
 *    on the ATmega168A. All implementation details (registers, data flow,
 *    usage patterns) are documented in the application notes listed above.
 *
 *============================================================================*/
#define SPI_MODE        0       // 0,1,2,3  (SRAM likes 0)
#define SPI_MSBFIRST    0       // 1=MSB first, 0=LSB first
#define SPI_PRESCALER   64       // 2,4,8,16,32,64,128
#define SPI_USE_IRQ     0       // 1=use SPI interrupt, 0=poll
#define SPI_SPE         1       // SPI Enable(1) Disable(0)
#define SPI_MSTR        1       // MSTR: Master(1)/Slave Select(0)

#define SPI_PORT        PORTB
#define SCK             PB5
#define MISO            PB4
#define MOSI            PB3
#define CS              PB0

#define START_SPI       (PORTB &= ~(1<<CS))
#define STOP_SPI        (PORTB |=  (1<<CS))


// ---- mode -> CPOL/CPHA ----
#if   (SPI_MODE==0)
  #define SPI_CPOL 0
  #define SPI_CPHA 0
#elif (SPI_MODE==1)
  #define SPI_CPOL 0
  #define SPI_CPHA 1
#elif (SPI_MODE==2)
  #define SPI_CPOL 1
  #define SPI_CPHA 0
#elif (SPI_MODE==3)
  #define SPI_CPOL 1
  #define SPI_CPHA 1
#else
  #error "SPI_MODE must be 0..3"
#endif

// ---- prescaler -> SPR1/SPR0 + SPI2X ----
#if   (SPI_PRESCALER==2)
  #define SPI_SPR1 0
  #define SPI_SPR0 0
  #define SPI_SPI2X 1
#elif (SPI_PRESCALER==4)
  #define SPI_SPR1 0
  #define SPI_SPR0 0
  #define SPI_SPI2X 0
#elif (SPI_PRESCALER==8)
  #define SPI_SPR1 0
  #define SPI_SPR0 1
  #define SPI_SPI2X 1
#elif (SPI_PRESCALER==16)
  #define SPI_SPR1 0
  #define SPI_SPR0 1
  #define SPI_SPI2X 0
#elif (SPI_PRESCALER==32)
  #define SPI_SPR1 1
  #define SPI_SPR0 0
  #define SPI_SPI2X 1
#elif (SPI_PRESCALER==64)
  #define SPI_SPR1 1
  #define SPI_SPR0 0
  #define SPI_SPI2X 0
#elif (SPI_PRESCALER==128)
  #define SPI_SPR1 1
  #define SPI_SPR0 1
  #define SPI_SPI2X 0
#else
  #error "SPI_PRESCALER must be one of {2,4,8,16,32,64,128}"
#endif

// ---- Interrupt Enable----
#if (SPI_USE_IRQ == 0)
    #define SPI_SPIE 0
#else
    #define SPI_SPIE 1
#endif

#if (SPI_MSBFIRST == 0)
    #define SPI_DORD 0
#else
    #define SPI_DORD 1
#endif
/*==============================================================================
 *  END OF SPI MODULE CONFIGURATION
*==============================================================================*/


void spiInitPoll();
void spiInitInt();
void spiWritePoll(char *data);
void spiWritePoll_(uint8_t *data, uint16_t len);
void spiWritePollByte_(uint8_t data);
void spiWriteInt(char *data);
void spiReadPoll(char *data, uint16_t size);
void spiReadPoll_(uint8_t *data, uint16_t len);
void spiReadPollByte_(uint8_t *data);
void spiReadInt(char *data, uint16_t size);


uint8_t spiWriteCheckPollByte_(uint8_t data);

#endif