#include <string.h>
#include "spi.h"

/*=============================================================================
 * SPI Protocol – Polling-Mode Initialization
 *
 * @summary
 *   Configures SPI hardware for master mode with polling (no interrupts).
 *   Sets GPIO directions, default chip-select idle state, and SPI registers.
 * 
 * Note: Do not change this code without notifying others, this configuration
 * is binded to the header file. To configure the SPI to your specific communication
 * change the parameters in the header: spi.h
 *============================================================================*/
void spiInitPoll(){
    // Set SCK, MOSI and CS direction as Output
    DDRB |= (1<<SCK) | (1<<MOSI) | (1<<CS) | (1<<MSTR);

    // Set SS pin HIGH (otherwise the master mode will be overwritten)
    PORTB |= (1<<PB2);

    // Set MISO direction as input (Driven by the slave)
    DDRB &= ~(1<<MISO) ;

    // Set CS HIGH (SPI protocol in idle mode)
    PORTB |= (1<<CS);

    SPCR &= ~(SPI_SPIE << SPIE);    // Enable/Disable Interrupt Mode
    SPCR |=  (SPI_SPE << SPE);      // Enable/Disable SPI
    SPCR |=  (SPI_DORD << DORD);    // Set Data Orientation
    SPCR |=  (SPI_MSTR << MSTR);    // Enable/Disable Master Mode
    SPCR |=  (SPI_CPOL << CPOL);    // Set Orientation for clock indle
    SPCR |=  (SPI_CPHA << CPHA);    // Set clock phase
    SPCR |=  (SPI_SPR1 << SPR1);    // Adjust prescaler 
    SPCR |=  (SPI_SPR0 << SPR0);    // Adjust prescaler 
    SPSR &= ~(SPI_SPI2X << SPI2X);  // Adjust prescaler 
}


/*=============================================================================
 * SPI Protocol – Interrupt-Driven Initialization
 *
 * @summary
 *   Configures SPI hardware for master mode using the SPI interrupt.
 *   Same electrical setup as polling, but enables SPIE for ISR-driven I/O.
 *============================================================================*/
// void spiInitInt(){
//     // Set SCK, MOSI and CS direction as Output
//     DDRB |= (1<<SCK) | (1<<MOSI) | (1<<CS) | (1<<MSTR);
//
//     // Set SS pin HIGH (otherwise the master mode will be overwritten)
//     PORTB |= (1<<PB2);
//
//     // Set MISO direction as input (Driven by the slave)
//     DDRB &= ~(1<<MISO) ;
//
//     // Set CS HIGH (SPI protocol in idle mode)
//     PORTB |= (1<<CS);
//
//     SPCR &= ~(SPI_SPIE << SPIE);    // Enable/Disable Interrupt Mode
//     SPCR |=  (SPI_SPE << SPE);      // Enable/Disable SPI
//     SPCR |=  (SPI_DORD << DORD);    // Set Data Orientation
//     SPCR |=  (SPI_MSTR << MSTR);    // Enable/Disable Master Mode
//     SPCR |=  (SPI_CPOL << CPOL);    // Set Orientation for clock indle
//     SPCR |=  (SPI_CPHA << CPHA);    // Set clock phase
//     SPCR |=  (SPI_SPR1 << SPR1);    // Adjust prescaler 
//     SPCR |=  (SPI_SPR0 << SPR0);    // Adjust prescaler 
//     SPSR &= ~(SPI_SPI2X << SPI2X);  // Adjust prescaler 
// }


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
void spiWritePoll(char *data){
    START_SPI;                         // CS low
    for(uint16_t i=0; i< strlen(data); i++){
        SPDR = (uint8_t)data[i];
        while(!(SPSR & (1<<SPIF))){}
    }
    STOP_SPI;
}


/*=============================================================================
 * SPI Protocol – Write (Polling) -- Type 2
 *
 * @summary
 *   Transmits a byte stream over SPI in master mode using pollin just as the above
 * function. However, the SPI start and stop control are handled by the caller. Also,
 * It is the caller responsibility to determine how many bytes are to be sent.

 *============================================================================*/
void spiWritePoll_(uint8_t *data, uint16_t len){
    for(uint16_t i=0; i< len; i++){
        SPDR = (uint8_t)data[i];
        while(!(SPSR & (1<<SPIF))){}
    }
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
// void spiWriteInt(char *data){
//     // TO DO
// }

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
void spiReadPoll(char *data, uint16_t size){
    START_SPI;
    for(uint16_t i=0; i<size; i++){
        SPDR = 0x00;
        while(!(SPSR & (1<<SPIF))){}       // Check flag to confirm the data is ready to be read.
        data[i] = SPDR;                    // Get data from buffer
    }
    STOP_SPI;
}


void spiReadPoll_(uint8_t *data, uint16_t len){
    START_SPI;
    for(uint16_t i=0; i<len; i++){
        SPDR = 0x00;
        while(!(SPSR & (1<<SPIF))){}       // Check flag to confirm the data is ready to be read.
        data[i] = SPDR;                    // Get data from buffer
    }
    STOP_SPI;
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
// void spiReadInt(char *data, uint16_t size){
//     // TO DO
// }

void spiWriteByte(uint8_t data) {
    START_SPI;
    SPDR = data;
    while(!(SPSR & (1 << SPIF)));
    STOP_SPI;
}
