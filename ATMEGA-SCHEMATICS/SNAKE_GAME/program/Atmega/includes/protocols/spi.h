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
#define SPI_MODEuu        0       // 0,1,2,3  (SRAM likes 0)
#define SPI_MSBFIRST    0       // 1=MSB first, 0=LSB first
#define SPI_PRESCALER   64       // 2,4,8,16,32,64,128
#define SPI_USE_IRQ     0       // 1=use SPI interrupt, 0=poll
#define SPI_SPE         1       // SPI Enable(1) Disable(0)
#define SPI_MSTR        1       // MSTR: Master(1)/Slave Select(0)

enum SPI_MODES{
    SPI_MASTER = 0X00,
    SPI_SLAVE = 0X01,
    SPI_MASTER_SLAVE = 0X02
};

typedef struct {
  volatile uint8_t  *CS_DDR;
  volatile uint8_t  *CS_PORT;
  uint8_t            CS_PIN;
}SPI_CS_TARGET;

typedef struct{
  uint8_t mode; // 0,1,2,3  (SRAM likes 0)
  uint8_t lsbfirst; // 1=MSB first, 0=LSB first
  uint8_t prescaler; // 2,4,8,16,32,64,128
  uint8_t irq; // 1=use SPI interrupt, 0=poll 
  uint8_t en;  // SPI Enable(1) Disable(0)
  uint8_t mstr; // SPI MODES
}SPI_MODE;

typedef struct{
  uint8_t cpol;
  uint8_t cpha;
  uint8_t spr2x;
  uint8_t spr1;
  uint8_t spr0;
  SPI_MODE *mode_conf;
}SPI_CONF;

typedef struct {
  SPI_CONF *conf;
  SPI_CS_TARGET  *cs_reg;
  char *buffer[100];
}SPI;

/*==============================================================================
 *  END OF SPI MODULE CONFIGURATION
*==============================================================================*/


void spiInit(SPI *spi);
void spiPause(SPI *spi);
void spiResume(SPI *spi);
void spiStart(SPI *spi);
void spiStop(SPI *spi);
void spiWritePoll(SPI *spi, char *data);
void spiWritePoll_(uint8_t *data, uint32_t len);
void spiWritePollByte_(uint8_t data);
void spiWriteInt(SPI *spi, char *data);
void spiReadPoll(SPI *spi, char *data, uint16_t size);
void spiReadPoll_(uint8_t *data, uint16_t len);
void spiReadPollByte_(uint8_t *data);
void spiReadInt(SPI *spi, char *data, uint16_t size);

uint8_t spiWriteCheckPollByte_(uint8_t data);

#endif