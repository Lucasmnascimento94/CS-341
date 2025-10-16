#include "start.h"
#include "./startSetUp/ws2812b/WS2812B.h"
#include "snake/snake.h"
#include "ws2812b/colors.h"
#include "startSetUp/sram/sram.h"
#include "i2c.h"
#include "screen.h"
#include "allocation.h"

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
    PORTC |= (1<<PC5) | (1<<PC4);

    //char c[20];
    LED_DDR |= (1 << LED_PIN);   // data pin as output
    //DDRB |= (1<<PB1);
    //PORTB &= ~(1<<PB1);
    //seed_prng();
    //gpioConfig();
    clear();


    //print("STARTING\n", 1);
   /* gameInit(&belly, &food);
    _delay_ms(500);
    initSnake(&belly, &food);
    belly.begin = true;
    push(&belly, PIXEL_ADDRESS(8, 5, 2), 8, 5, false);
    uint32_t addr = 0X00;

    uint16_t len_text = (uint16_t)(sizeof(c) - 1);
    uint16_t len_msg  = (uint16_t)(sizeof(msg)  - 1);
    uint16_t len_msg2 = (uint16_t)(sizeof(msg2) - 1);
    uint8_t buffer[300];         
    uint16_t len = sizeof(c);
    sramWriteModeRegister(SRAM_MODE_SEQU);
    sramReadModeRegister();
    sramWriteStringPoll(0x00, 0xFF, len);
    _delay_ms(10);
    */
    
    //screenInit(&i2c_port, &i2c_target);


    char c[30];
    sprintf(c, "HELLO WORLD SNAKE PROJECT");
    uartWrite_(c);
    _delay_ms(100);
    memset(i2c_port.data, 0, strlen(i2c_port.data));
    i2c_port.data = c;
    //screenWrite(&i2c_port, &i2c_target, &screen);
    //sprintf(c, "THIS IS ADDR IN CONF: %X\n", i2c_conf.slave[0]->addr);
   // uartWrite_(c);

    while(1){
        test();
        uartWrite("lOOPING\n", 9);
        _delay_ms(500);
    }
}
