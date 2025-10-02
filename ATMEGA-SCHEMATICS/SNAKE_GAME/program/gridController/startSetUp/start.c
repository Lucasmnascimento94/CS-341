#include "start.h"


void gpioConfig(){
    joyStickConf();
    setUpUART();
    setUpSPI();
}


void joyStickConf(){
    /*Pin Data Direction*/
    JOYSTICK_DDR &= ~((1<<JOYSTICK_UP) | (1<<JOYSTICK_DOWN) | (1<<JOYSTICK_LEFT) | (1<<JOYSTICK_RIGHT));
    JOYSTICK_PORT |=  ((1<<JOYSTICK_UP)|(1<<JOYSTICK_DOWN)|(1<<JOYSTICK_LEFT)|(1<<JOYSTICK_RIGHT));

    /*ISR Conf.*/   
    PCIFR |= (1<<PCIF2);    // Clear flag
    PCMSK2 |= (1<<PCINT21) | (1<<PCINT20) | (1<<PCINT19) | (1<<PCINT18);
    PCICR |= (1<< PCIE2);
    sei();
}

void setUpUART(){
    UBRR0H = (unsigned char)(MYUBRR>>8);
    UBRR0L = (unsigned char)MYUBRR;

    UCSR0C &= ~((1<<UMSEL01)|(1<<UMSEL00) | (1<<UPM01) | (1<<UPM00)); // Asynchronous Mode
    UCSR0C |= (1<<USBS0);// 2-STOP BITS
    
    UCSR0A &= ~(1<<U2X0); 

    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    /* Set frame format: 8data, 2stop bit */
   
    /*8 BIT CHARACTER*/
    UCSR0B &= ~(1<<UCSZ02);
    UCSR0C |= (1<<UCSZ01) |(1<<UCSZ00);
}




// The following Values can be adjusted at: start.h file

/* SPCR - SPI CONTROL REGISTER:
BIT_7 == SPIE: SPI Interrupt Enable
BIT_6 == SPE: SPI Enable
BIT_5 == DORD: Data Order
BIT_4 == MSTR: Master/Slave Select
BIT_3 == CPOL: Clock Polarity
BIT_2 == CPHA: Clock Phase
BIT_1 == SPR1: Clock rate
BIT_0 == SPR0: CLock rate
*/


/* SPSR – SPI Status Register
BIT_7 == SPIF: SPI Interrupt Flag
BIT_6 == WCOL: Write COLlision Flag
BIT_[5:1] == Reserved
BIT_0 == SPI2X: Double SPI Speed Bit

*/

/* SPDR – SPI Data Register
The SPI Data Register is a read/write register used for data transfer between the Register File and the SPI Shift
Register. Writing to the register initiates data transmission. Reading the register causes the Shift Register
Receive buffer to be read.
*/

void setUpSPI(){
    DDRB |= (1<<SCK) | (1<<MOSI) | (1<<CS) | (1<<MSTR);
    PORTB |= (1<<PB2);
    DDRB &= ~(1<<MISO) ;

    PORTB |= (1<<CS);

    SPCR |= (SPI_SPIE << SPIE);
    SPCR |= (SPI_SPE << SPE);
    SPCR |= (SPI_DORD << DORD);
    SPCR |= (SPI_MSTR << MSTR);
    SPCR |= (SPI_CPOL << CPOL);
    SPCR |= (SPI_CPHA << CPHA);
    SPCR |= (SPI_SPR1 << SPR1);
    SPCR |= (SPI_SPR0 << SPR0);
    SPSR &= ~(SPI_SPI2X << SPI2X);
}

void seed_prng(void){ srand(0xA5A5 ^ (uint16_t)TCNT0);}