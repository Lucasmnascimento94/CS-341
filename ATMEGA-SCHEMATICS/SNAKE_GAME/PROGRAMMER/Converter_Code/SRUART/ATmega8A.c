#include "Programmer.h"
#include "string.h"
#include <stdio.h>
#include <util/delay.h>

void init_uart() {
    UART_init(MYUBRR); // Check uart.c


    uartReadInt();
    // ISR(UART_INT){
 //   read
 CS -> GRID
 CS -> FLASH
 CS -> GA,E
 CS ->SRAM

 // device select
 // The road is already made. I just need to specify to the ATmega8 which route to take (from a function)

 // Make sure analog swtiches because talk to 1 at a time
//}

}


int main() {
    init_uart();   
    uartWrite_size("Hello, World!", 13);
}