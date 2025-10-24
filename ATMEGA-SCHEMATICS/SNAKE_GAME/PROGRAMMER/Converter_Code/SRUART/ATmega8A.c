#include "uart.h"
#include "string.h"
#include <stdio.h>
#include <util/delay.h>

void init_uart() {
    UART_init(MYUBRR); // Check uart.c
}



int main() {
    init_uart();   
}