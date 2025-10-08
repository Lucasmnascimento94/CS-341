#include "start.h"
#include "./startSetUp/ws2812b/WS2812B.h"
#include "snake/snake.h"
#include "ws2812b/colors.h"
#include "startSetUp/sram/sram.h"

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
    gameInit(&belly, &food);
    _delay_ms(500);
    initSnake(&belly, &food);
    belly.begin = true;
    push(&belly, PIXEL_ADDRESS(8, 5, 2), 8, 5, false);
    uint32_t addr = 0X00;

    sramWriteModeRegister(SRAM_MODE_SEQU);
    sramReadModeRegister();
    while(1){
        //uint8_t c;
        //sramWriteByte(0x48, addr);
        //walk(&belly, &food);
        //if(belly.end) gameEnd(&belly, &food);
        uint8_t c[]="HELLO WORLD FROM SRAM";
        uint16_t len = strlen(c);
        uint8_t buffer[50] = {};
        sramWriteStringPoll(c, 0xFF);
        _delay_ms(10);

        sramReadString(buffer, len, 0xFF);
        //sramReadByte(&c, addr);
        //spiWritePoll("HELLO");
    }
}
