#include "start.h"
#include "WS2812B.h"
#include "print.h"
#include "snake/snake.h"
#include "snake/colors.h"
#include "stdio.h"

SnakeBelly belly = {
    .color = COLOR_GOLD,
    .color_food = COLOR_GREEN,
    .count = 0,
    .direction =LEFT,
    .head = NULL,
    .tail = NULL,
    .walk = false,
    .end = false,
    .begin = false,
    .pt = 0
};

struct Cell food = {
    .i = 8,
    .j = 5,
    .next = NULL,
    .poison = false,
    .prev = NULL,
    .val = PIXEL_ADDRESS(8, 5, 2) 
};

/*
DDRX -> DIRECTION
1 -> clock
*/

void sendZero(){
    PORTD &= ~((1<<PD0));

    PORTD &= ~((1<<PD1));
    __builtin_avr_delay_cycles(4);
    PORTD |= ((1<<PD1));
    __builtin_avr_delay_cycles(4);
    PORTD |= ((1<<PD0));
}


void sendOne(){
    PORTD |= ((1<<PD0));

    PORTD &= ~((1<<PD1));
    __builtin_avr_delay_cycles(4);
    PORTD |= ((1<<PD1));
    __builtin_avr_delay_cycles(4);
    PORTD |= ((1<<PD0));
}


void start(){
    for(int i=0; i<32; i++){
        sendZero();
    }   
}

void stop(){
        for(int i=0; i<32; i++){
        sendOne();
    }   
}

void sendRed(){
    start();
    for (int i=0; i < 5; i++){
        for(int j=0; j<8; j++){

            sendOne();
        }
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

    stop();
}

void sendGreen(){
    start();
    for (int i=0; i < 5; i++){
        for(int j=0; j<8; j++){

            sendOne();
        }
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

    stop();
}


void sendWhite(){
    start();
    for (int i=0; i < 5; i++){
        for(int j=0; j<32; j++){

            sendOne();
        }
    }

    stop();
}
void sendBlue(){
    start();
    for (int i=0; i < 5; i++){
        for(int j=0; j<8; j++){

            sendOne();
        }
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

    stop();
}


int main(void){
    //char c[20];
    LED_DDR |= (1 << LED_PIN);   // data pin as output
    //seed_prng();
    gpioConfig();
    //clear();
    DDRB |= (1<<PIN1);
    PORTB &= ~(1<<PIN1);

    DDRD |= ((1<<PD0) | (1<<PD1));
    
    PORTD |= ((1<<PD1) | (1<<PD0));
    //print("STARTING\n", 1);
    //initSnake(&belly);
    //gameInit(&belly, &food);
    _delay_ms(500);
    //initSnake(&belly, &food);
    //belly.begin = true;
    //push(&belly, PIXEL_ADDRESS(8, 5, 2), 8, 5, false);
    start();
    while(1){
        //print("LOOPING\n", 1);
        //walk(&belly, &food);
        //PORTD |= ((1<<PD1) | (1<<PD0));
        //if(belly.end) gameEnd(&belly, &food);
        sendRed();
        _delay_ms(50);
        sendWhite();
        _delay_ms(50);
        sendGreen();
        _delay_ms(50);
        sendWhite();
        _delay_ms(50);
        sendBlue();
        _delay_ms(50);
        sendWhite();
        _delay_ms(50);
        //PORTD &= ~((1<<PD1) | (1<<PD0));
        //_delay_ms(500);
        
    }
}
