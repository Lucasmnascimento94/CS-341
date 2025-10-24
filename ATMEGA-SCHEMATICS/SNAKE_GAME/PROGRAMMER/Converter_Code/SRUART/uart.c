#include "uart.h"
#include "string.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/ATmega8A.h>
#include <avr/pgmspace.h> // Program Memory (Flash) 


//-------------------------------------//
// Write a string of a given size to the UART
//-------------------------------------//
void uartWrite_size(char *c, uint16_t size){
}

//-------------------------------------//
// Write a string to the UART
//-------------------------------------//
void uartWrite_strlen(char *c){
}

//-------------------------------------//
// Read a string from the UART
//-------------------------------------//
void uartRead(char *c){
}


//-------------------------------------//
// Send a byte SLAVES
//-------------------------------------//
void send_byte(uint8_t data) {
}

//-------------------------------------//
// Initialize the UART
//-------------------------------------//
void UART_init(unsigned int ubrr) {
    // Set baud rate
    // UBRRL and UBRRH – USART Baud Rate Registers
    //The ATmega8A has two separate 8-bit registers for baud rate:
    //UBRRH for the high byte and UBRRL for the low byte.
    // 16 bit value is split into two 8 bit values
    // Example: ubrr = 16 = 0001 1010 0010 1011 (0x1A2B)
    // UBRRH = 0001 1010 (0x1A)
    // UBRRL = 0010 1011 (0x2B)
    UBRRH = (unsigned char)(ubrr>>8); // High Byte (MSB)
    UBRRL = (unsigned char)ubrr; // Low Byte (LSB)

    

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

}