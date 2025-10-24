#ifndef UART_H
#define UART_H


#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/eeprom.h>


#define FOSC      8000000 // 8MHz Clock Speed
#define BAUD_RATE 9600 // 9600 Baud Rate
#define MYUBRR    (FOSC/(8*BAUD_RATE) - 1) // Asynchronous Double Speed Mode For ATmega8A

void uartWrite_size(char *c, uint16_t size); // Write a string of a given size to the UART
void uartWrite_strlen(char *c); // Write a string to the UART
void uartRead(char *c); // Read a string from the UART
void UART_init(unsigned int ubrr); // Initialize the UART
void send_byte(uint8_t data); // Send a byte to SLAVES

#endif