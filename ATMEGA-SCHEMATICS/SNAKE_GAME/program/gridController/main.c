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



int main(void){
    //char c[20];
    LED_DDR |= (1 << LED_PIN);   // data pin as output
    //DDRB |= (1<<PB1);
    //PORTB &= ~(1<<PB1);
    //seed_prng();
    gpioConfig();
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
    
    screen.instruction = malloc(sizeof(char)*6);
    i2cConfig(&i2c_conf);
    screenInit(&i2c_conf, &i2c_slave, &screen);


    char c[20];
    sprintf(c, "HELLO WORLD SNAKE PROJECT:", i2c_slave.addr);
    uartWrite_(c);
    _delay_ms(100);
    memset(i2c_conf.data, 0, sizeof(i2c_conf.data));
    i2c_conf.data = c;
    screenWrite(&i2c_conf, &screen);
    //sprintf(c, "THIS IS ADDR IN CONF: %X\n", i2c_conf.slave[0]->addr);
   // uartWrite_(c);

    while(1){

        uartWrite("lOOPING\n", 9);
        _delay_ms(5000);
    }
}
