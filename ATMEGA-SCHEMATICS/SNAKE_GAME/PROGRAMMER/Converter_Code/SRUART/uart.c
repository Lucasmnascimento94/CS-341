#include "Programmer.h"
#include "string.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h> // Program Memory (Flash) 


//-------------------------------------//
// Write a string of a given size to the UART
//-------------------------------------//
// UDRE: USART Data Register Empty
void uartWrite_size(char *c, uint16_t size){
    for (uint16_t i = 0; i < size; i++){
        while (!(UCSRA & (1<<UDRE)));
        UDR = (uint8_t)c[i];
    }
}

//-------------------------------------//
// Write a string to the UART
//-------------------------------------//
// UDRE: USART Data Register Empty
void uartWrite_strlen(char *c){
    for (uint16_t i = 0; i < strlen(c); i++){
        while (!(UCSRA & (1<<UDRE)));
        UDR = (uint8_t)c[i];
    }
}

//-------------------------------------//
// Read a string from the UART
//-------------------------------------//
// RXC: USART Receive Complete
void uartRead(void){
    while (!(UCSRA & (1<<RXC)));
    return UDR;
}

//-------------------------------------//
// Receive frames with 5 to 8 data bits
//-------------------------------------//
// RXC: USART Receive Complete
unsigned char UART_Receive(void) {
    /* Wait for data to be received */
    while (!(UCSRA & (1<<RXC)));
    /* Get and return received data from buffer */
    return UDR;
}

//-------------------------------------//
// Read From Register UCSRC
//-------------------------------------//
// USART Control and Status Register C
unsigned char UART_ReadUCSRC(void) {
    unsigned char ucsrc;
    ucsrc = UBRRH;
    ucsrc = UCSRC
    return ucsrc;
}

//-------------------------------------//
// Initialize the UART
//-------------------------------------//
void UART_init(unsigned int ubrr) {
    // Set baud rate
    // UBRRL and UBRRH – USART Baud Rate Registers
    // The ATmega8A has two separate 8-bit registers for baud rate:
    // UBRRH for the high byte and UBRRL for the low byte.
    // 16 bit value is split into two 8 bit values
    // Example: ubrr = 16 = 0001 1010 0010 1011 (0x1A2B)
    // UBRRH = 0001 1010 (0x1A)
    // UBRRL = 0010 1011 (0x2B)
    // UCSRA: USART Control and Status Register A
    UBRRH = (unsigned char)(ubrr>>8); // High Byte (MSB)
    UBRRL = (unsigned char)ubrr; // Low Byte (LSB)
    UCSRA &= ~(1<<U2X); // Clear U2X flag to 
    
    // Enable Receiver and Transmitter
    // RXEN: Receiver Enable
    // TXEN: Transmitter Enable
    // UCSZ2: Character Size Combined with UCSZ0 and UCSZ1 (8 bits)
    UCSRB = (1<<RXEN) | (1<<TXEN);

    // Set frame format: 8data, 1stop bit
    // UMSEL: USART Mode Select (0: Asynchronous Mode)
    // USBS: Stop Bit Select (0: 1 stop bit)
    // UCSZ0: Character Size 
    // UCSZ1: Character Size
    // USCZ2 + UCSZ0 + UCSZ1 (8 bits)
    UCSRC = (0 << UMSEL) | (0 <<USBS) | (1<<UCSZ0) | (1<<UCSZ1) | (0<<UCSZ2);

    // Phai co function de luu vao trong microcontroller (Address)
}