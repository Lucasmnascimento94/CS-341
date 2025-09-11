#include "start.h"
#include "WS2812B.h"

SnakeBelly belly = {
    .color_food = 0x00,
    .count = 5,
    .direction = UP,
    .head = NULL,
    .tail = NULL,
    .walk = false
};

void seed_prng(void){ srand(0xA5A5 ^ (uint16_t)TCNT0); }


int main(void){
    LED_DDR |= (1 << LED_PIN);   // data pin as output
    seed_prng();
    gpioConfig();

    while(1){
        //loadGame(belly);
        if(belly.walk){
            snake(10);
            belly.walk = false;
        }
        //playColor();
        _delay_ms(1);
        clear();
        _delay_ms(1);
    }

    
}
