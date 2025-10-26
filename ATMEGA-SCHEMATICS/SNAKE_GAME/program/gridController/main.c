#include "start.h"
#include "uart.h"
/*
DDRX -> DIRECTION
*/

void sendOne(){
    PORTC |= (1<<PC5) | (1<<PC4);
    _delay_us(1);
    PORTC &= ~(1<<PC5);
    _delay_us(1);
}

void sendZero(){
    PORTC &= ~(1<<PC4);
    PORTC |= (1<<PC5);
    _delay_us(1);
    PORTC &= ~(1<<PC5);
    _delay_us(1);
}

void sendBright(){
    sendOne();
    sendOne();
    sendOne();

    sendZero();
    sendZero();
    sendZero();
    sendOne();
    sendOne();
    
}
void test(){
    for(int i=0; i<32;i++){
        sendZero();
    }

    for(int i=0; i<196; i++){
        sendBright();
        for(int j=0; j<8; j++){
            sendOne();
        }
        for(int j=0; j<8; j++){
            sendZero();
        }
        for(int j=0; j<8; j++){
            sendZero();
        }
    }

    for(int i=0; i<32;i++){
        sendOne();
    }

      _delay_ms(1000);




      for(int i=0; i<32;i++){
        sendZero();
    }

    for(int i=0; i<196; i++){
        sendBright();
        for(int j=0; j<8; j++){
            sendZero();
        }
        for(int j=0; j<8; j++){
            sendOne();
        }
        for(int j=0; j<8; j++){
            sendZero();
        }
    }

    for(int i=0; i<32;i++){
        sendOne();
    }

      _delay_ms(1000);








      for(int i=0; i<32;i++){
        sendZero();
    }

    for(int i=0; i<196; i++){
        sendBright();
        for(int j=0; j<8; j++){
            sendZero();
        }
        for(int j=0; j<8; j++){
            sendZero();
        }
        for(int j=0; j<8; j++){
            sendOne();
        }
    }

    for(int i=0; i<32;i++){
        sendOne();
    }

      _delay_ms(1000);

    for(int i=0; i<32;i++){
        sendZero();
    }
    
    for(int i=0; i<196; i++){
        sendBright();
        for(int j=0; j<24; j++){
            sendZero();
        }
    }
    for(int i=0; i<32;i++){
        sendOne();
    }
}


int main(void){

    DDRC |= (1<<PC5) | (1<<PC4);
    LED_DDR |= (1 << LED_PIN);  
    //DDRB |= (1<<PB1);
    //PORTB &= ~(1<<PB1);
    //seed_prng();
    gpioConfig();


    char c[30];
    sprintf(c, "HELLO WORLD SNAKE PROJECT");
    uartWrite_(c);
    _delay_ms(100);
    while(1){
        test();
        uartWrite("lOOPING\n", 9);
        _delay_ms(500);
    }
}
