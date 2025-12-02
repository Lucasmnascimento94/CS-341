
#include "spi.h"
#include "string.h"
#include "WS2812B.h"

static inline void dcy_(uint16_t c){__builtin_avr_delay_cycles(c);}
/*=============================================================================
 * SPI Protocol Initialization
 * SPI struct needs to be configured before calling this function
 * @summary
 *   Configures SPI hardware for master mode with polling (no interrupts).
 *   Sets GPIO directions, default chip-select idle state, and SPI registers.
 * 
 * Note: Do not change this code without notifying others, this configuration
 * is binded to the header file. To configure the SPI to your specific communication
 * change the parameters in the header: spi.h
 *============================================================================*/
void spiConf(SPI *spi);

void spiInit(SPI *spi){
    
    spiConf(spi);
    DDRB |= (1<<PB5) | (1<<PB3) | (1<<PB2);
    DDRB &= ~(1<<PB4);

    PORTB |= (1<<PB5) | (1<<PB4) | (1<<PB3) | (1<<PB2);

    /*CS not static to accomodate different targets based on package command*/
    *spi->cs_reg->CS_DDR        |= (1<<spi->cs_reg->CS_PIN);
    *spi->cs_reg->CS_PORT       |= (1<<spi->cs_reg->CS_PIN);

    
    SPCR =  (SPCR & ~_BV(SPIE)) | ((spi->conf->mode_conf->irq& 1u) << SPIE);       // Enable/Disable Interrupt Mode
    SPCR =  (SPCR & ~_BV(DORD)) | ((spi->conf->mode_conf->lsbfirst & 1u)<< DORD);                  // Set Data Orientation
    SPCR =  (SPCR & ~_BV(MSTR)) | ((spi->conf->mode_conf->mstr & 1u)<< MSTR);       // Enable/Disable Master Mode
    SPCR =  (SPCR & ~_BV(CPOL)) | ((spi->conf->cpol & 1u)<< CPOL);                  // Set Orientation for clock indle
    SPCR =  (SPCR & ~_BV(CPHA)) | ((spi->conf->cpha & 1u)<< CPHA);                  // Set clock phase
    SPCR =  (SPCR & ~_BV(SPR1)) | ((spi->conf->spr1 & 1u)<< SPR1);                  // Adjust prescaler 
    SPCR =  (SPCR & ~_BV(SPR0)) | ((spi->conf->spr0 & 1u)<< SPR0);                  // Adjust prescaler 
    SPSR =  (SPSR & ~_BV(SPI2X)) | ((spi->conf->spr2x & 1u) << SPI2X);              // Adjust prescaler 

    SPCR =  (SPCR & ~_BV(SPE))  | ((spi->conf->mode_conf->en & 1u)<< SPE);           // Enable/Disable SPI
}

void spiPause(SPI *spi){
    spi->conf->mode_conf->en = false;
    DDRB &= ~(1<<PB3) & ~(1<<PB2);
    PORTB |= (1<<PB3) | (1<<PB2);

    spiStop(spi);
    SPCR &=  ~(SPCR & ~_BV(SPE))  & ~((spi->conf->mode_conf->en & 1u)<< SPE);   
}

void spiResume(SPI *spi){
    spi->conf->mode_conf->en = true;
    spiInit(spi);  
}

void spiStart(SPI *spi){
    *spi->cs_reg->CS_DDR |= (1<<spi->cs_reg->CS_PIN);
    *spi->cs_reg->CS_PORT &= ~(1<<spi->cs_reg->CS_PIN);
}

void spiStop(SPI *spi){
    *spi->cs_reg->CS_DDR &= ~(1<<spi->cs_reg->CS_PIN);
    *spi->cs_reg->CS_PORT |= (1<<spi->cs_reg->CS_PIN);
}

void spiConf(SPI *spi){
    switch (spi->conf->mode_conf->mode){
        case 0:
            spi->conf->cpol = 0;
            spi->conf->cpha = 0;
            break;
        case 1:
            spi->conf->cpol = 0;
            spi->conf->cpha = 1;
            break;
        case 2:
            spi->conf->cpol = 1;
            spi->conf->cpha = 0;
            break;
        case 3:
            spi->conf->cpol = 1;
            spi->conf->cpha = 1;
            break;
    }

    switch (spi->conf->mode_conf->prescaler){
        case 2:
            spi->conf->spr1 = 0;
            spi->conf->spr0 = 0;
            spi->conf->spr2x = 1;
            break;
        case 4:
            spi->conf->spr1 = 0;
            spi->conf->spr0 = 0;
            spi->conf->spr2x = 0;
            break;
        case 8:
            spi->conf->spr1 = 0;
            spi->conf->spr0 = 1;
            spi->conf->spr2x = 1;
            break;
        case 16:
            spi->conf->spr1 = 0;
            spi->conf->spr0 = 1;
            spi->conf->spr2x = 0;
            break;
        case 32:
            spi->conf->spr1 = 1;
            spi->conf->spr0 = 0;
            spi->conf->spr2x = 1;
            break;
        case 64:
            spi->conf->spr1 = 1;
            spi->conf->spr0 = 0;
            spi->conf->spr2x = 0;
            break;
        case 128:
            spi->conf->spr1 = 1;
            spi->conf->spr0 = 1;
            spi->conf->spr2x = 0;
            break;
    }
}

/*=============================================================================
 * SPI Protocol – Write (Polling)
 *
 * @summary
 *   Transmits a byte stream over SPI in master mode using polling.
 *
 * @params
 *   data : Pointer to a null-terminated buffer to send (excludes the '\0').
 *
 * @pre
 *   - SPI initialized (spiInitPoll()).
 *   - CS is asserted low by caller before write; deasserted after.
 *============================================================================*/
void spiWritePoll(SPI *spi, char *data){

    spiStart(spi);                         // CS low
    for(uint16_t i=0; i< strlen(data); i++){
        SPDR = (uint8_t)data[i];        
        while(!(SPSR & (1<<SPIF))){}
    }
    spiStop(spi);             
}


/*=============================================================================
 * SPI Protocol – Write (Polling) -- Type 2
 *
 * @summary
 *   Transmits a byte stream over SPI in master mode using pollin just as the above
 * function. However, the SPI start and stop control are handled by the caller. Also,
 * It is the caller responsibility to determine how many bytes are to be sent.

 *============================================================================*/
void spiWritePoll_(uint8_t *data, uint32_t len){
    for(uint16_t i=0; i< len; i++){
        SPDR = (uint8_t)data[i];        
        while(!(SPSR & (1<<SPIF))){}
    }     
}

void spiWritePollByte_(uint8_t data){
    SPDR = data;        
    while(!(SPSR & (1<<SPIF))){}
}

uint8_t spiWriteCheckPollByte_(uint8_t data){
    SPDR = data;        
    while(!(SPSR & (1<<SPIF))){}
    return SPDR;
}


/*=============================================================================
 * SPI Protocol – Write (ISR)
 *
 * @summary
 *   Transmits a byte stream over SPI in master mode using Interrupt Service Routine.
 *
 * @params
 *   data : Pointer to a null-terminated buffer to send (excludes the '\0').
 *
 * @pre
 *   - SPI initialized (spiInitInt()).
 *   - CS is asserted low by caller before write; deasserted after.
 *============================================================================*/
/*void spiWriteInt(SPI *spi, char *data){
    // TO DO
}*/

/*=============================================================================
 * SPI Protocol – Read (Polling)
 *
 * @summary
 *   Receives a fixed number of bytes over SPI in master mode using polling.
 *
 * @params
 *   data : Pointer to destination buffer (size >= 'size').
 *   size : Number of bytes to read from the slave.
 *
 * @pre
 *   - SPI initialized (spiInitPoll()).
 *   - CS is asserted low by caller before read; deasserted after.
 *   - Slave prepared to shift data out (master will clock by writing dummy 0xFF).
 *============================================================================*/
void spiReadPoll(SPI *spi, char *data, uint16_t size){

    spiStart(spi);
    for(uint16_t i=0; i<size; i++){
        SPDR = 0x00;
        while(!(SPSR & (1<<SPIF))){}       // Check flag to confirm the data is ready to be read.
        data[i] = SPDR;                       // Get data from buffer
    }
    spiStop(spi);   
}

void spiReadPoll_( uint8_t *data, uint16_t len){
    for(uint16_t i=0; i<len; i++){
        SPDR = 0x00;
        while(!(SPSR & (1<<SPIF))){}       // Check flag to confirm the data is ready to be read.
        data[i] = SPDR;                       // Get data from buffer
    }
}

void spiReadPollByte_(uint8_t *data){
    SPDR = 0x00;
    while(!(SPSR & (1<<SPIF))){}       // Check flag to confirm the data is ready to be read.
    *data = SPDR;                       // Get data from buffer

}


void spiReadBufferWs2812b(uint32_t size){
    uint8_t data = 0x00;
    for(uint32_t i=0; i<size*3; i++){
        SPDR = 0x00;
        while(!(SPSR & (1<<SPIF))){}       // Check flag to confirm the data is ready to be read.
        data = SPDR;                       // Get data from buffer
        send_byte(data);
    }
    latch();
}

/*=============================================================================
 * SPI Protocol – Read (Polling)
 *
 * @summary
 *   Receives a fixed number of bytes over SPI in master mode using polling.
 *
 * @params
 *   data : Pointer to destination buffer (size >= 'size').
 *   size : Number of bytes to read from the slave.
 *
 * @pre
 *   - SPI initialized (spiInitInt()).
 *   - CS is asserted low by caller before read; deasserted after.
 *   - Slave prepared to shift data out (master will clock by writing dummy 0xFF).
 *============================================================================*/
/*void spiReadInt(SPI *spi, char *data, uint16_t size){
    // TO DO
}*/
