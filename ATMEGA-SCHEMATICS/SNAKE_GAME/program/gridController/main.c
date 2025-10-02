#include "./startSetUp/start.h"
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
    //seed_prng();
    gpioConfig();
    //clear();
   // DDRC |= (0<<PC6);
   // PORTC |= (1<<PC6);

    //print("STARTING\n", 1);
    gameInit(&belly, &food);
    _delay_ms(500);
    initSnake(&belly, &food);
    belly.begin = true;
    push(&belly, PIXEL_ADDRESS(8, 5, 2), 8, 5, false);

    while(1){
        uint8_t c;
        //print("LOOPING 123\n", 1);
        //walk(&belly, &food);

        //if(belly.end) gameEnd(&belly, &food);
        writeByte('H', 0XFF);
        _delay_ms(100);
        readByte(&c, 0xFF);
        _delay_ms(100);
    }
}
