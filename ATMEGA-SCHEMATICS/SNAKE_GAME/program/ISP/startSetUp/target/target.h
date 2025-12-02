#ifndef TARGET_H
#define TARGET_H
#include "includes.h"
#include "spi.h"
#include "uart.h"


/* Flash Conf. target*/
#define FLASH_CS_PIN        PD7
#define FLASH_CS_PORT       PORTD
#define FLASH_CS_DDR        DDRD
#define FLASH_PIN           PC1
#define FLASH_PORT          PORTC       
#define FLASH_DDR           DDRC

/* Sram Conf. target*/
#define SRAM_CS_PIN        PD5
#define SRAM_CS_PORT       PORTD
#define SRAM_CS_DDR        DDRD
#define SRAM_PIN           PC0
#define SRAM_PORT          PORTC       
#define SRAM_DDR           DDRC

/* Grid Conf. target*/
#define GRID_CS_PIN        PD2
#define GRID_CS_PORT       PORTD
#define GRID_CS_DDR        DDRD
#define GRID_PIN           PC0
#define GRID_PORT          PORTC       
#define GRID_DDR           DDRC

/* Game Conf. target*/
#define GAME_CS_PIN        PD3
#define GAME_CS_PORT       PORTD
#define GAME_CS_DDR        DDRD
#define GAME_PIN           PC1
#define GAME_PORT          PORTC       
#define GAME_DDR           DDRC

typedef struct {
    volatile uint8_t  *SIGNAL_DDR;
    volatile uint8_t  *SIGNAL_PORT;
    uint8_t            SIGNAL_PIN;
}TARGET_SIGNAL;

typedef struct {
    SPI_CONF        *spi_conf;
    SPI_CS_TARGET   *cs_reg;
    TARGET_SIGNAL   *signal;
}TARGET_CONF;

void targetSpiUpdate(SPI *spi, TARGET_CONF *new_target);
#endif