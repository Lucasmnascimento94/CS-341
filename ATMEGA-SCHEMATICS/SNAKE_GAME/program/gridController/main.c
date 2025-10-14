#include "start.h"
#include "./startSetUp/ws2812b/WS2812B.h"
#include "snake/snake.h"
#include "ws2812b/colors.h"
#include "startSetUp/sram/sram.h"
#include "i2c.h"
#include "screen.h"

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

static char c[]= "In a small coastal town, the rhythm of life followed the tides. "
                    "Every morning, fishermen set sail before sunrise, "
                    "their boats cutting through the mist as gulls cried overhead.";
static char msg[]  = "MESSAGE SENT\r\n";
static const char msg2[] = "MESSAGE READ\r\n";
/*
DDRX -> DIRECTION


*/

I2C_SLAVE i2c_slave = {
    .addr = PCF8574_ADDR_WRITE,
    .count = slave_count
};

I2C_CONF i2c_conf = {
    .data = {0},
    .interrupt_mode = false,
    .master = {0},
    .master_mode = true,
    .polling_mode = true,
    .prescaler = 1,
    .read_mode = false,
};

SCREEN_CONF screen = {
    .pcf8574_addr = PCF8574_ADDR_WRITE
};


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
    
    i2cConfig(&i2c_conf);
    screenInit(&i2c_conf, &i2c_slave, &screen);


    char c[50];
    sprintf(c, "THIS IS ADDR IN SLAVE: %X\n", i2c_slave.addr);
    uartWrite_(c);
    //sprintf(c, "THIS IS ADDR IN CONF: %X\n", i2c_conf.slave[0]->addr);
   // uartWrite_(c);

    while(1){
        //uartWrite(msg,  len_msg);
        //uartWrite(c, len_text);
        //uartWrite("\r\n\r\n\r\n", 6);   // exactly 6 bytes
        //_delay_ms(5);

        // (Re)write then read back
        //sramWriteStringPoll(c, 0xFF, len_text);
        //_delay_ms(5);
        //sramReadString(buffer, len_text, 0xFF);

        //uartWrite(msg2, len_msg2);
        //uartWrite((const char*)buffer, len_text);
        uartWrite("lOOPING\n", 9);

        (&i2c_conf, 0);
        _delay_ms(5000);
    }
}
