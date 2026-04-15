#ifndef UART_H
#define UART_H

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
#define FOSC      20000000UL
#define BAUD_RATE 9600UL
#define MYUBRR    (FOSC/(16UL*BAUD_RATE) - 1) 
/*==============================================================================
 *  END OF UART MODULE CONFIGURATION
*==============================================================================*/

void uartInit();
void uartWrite(char *c, uint16_t size);
void uartWrite_(char *c);

#endif