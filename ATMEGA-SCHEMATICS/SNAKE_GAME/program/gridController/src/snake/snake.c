#include "snake.h"
#include "WS2812B.h"
#include "uart.h"

#if (GAME == SNAKE)
/*============================================================================================*
 * RENDER — framebuffer/bitset and scanout                                                     *
 *============================================================================================*/
void walk(SnakeBelly *belly, struct Cell *food){
    if(belly == NULL || food == NULL) return;

    uint8_t new_i = belly->head->i;
    uint8_t new_j = belly->head->j;
    bool eatFood = false;

    switch (belly->direction){
        case UP:    new_j++; break;
        case DOWN:  new_j--; break;
        case LEFT:  new_i--; break;
        case RIGHT: new_i++; break;
    }

    if(new_i>48){new_i=1;}
    else if(new_i == 0){new_i=48;}

    if(new_j>32){new_j=1;}
    else if(new_j == 0){new_j=32;}

    uint16_t k = CURRENT_PAD(new_i);
    uint16_t I = LOCAL_I(new_i, k);

    if(!ruleCheck(belly, PIXEL_ADDRESS(I, new_j, k))){
        belly->end = true;
        return;
    }

    if(!foodCheck(belly, food, PIXEL_ADDRESS(I, new_j, k), &eatFood)){
        belly->end = true;
        return;
    }

    struct Cell *temp;

    if(!eatFood){
        for(temp= belly->tail; temp->prev != NULL; temp = temp->prev){
        temp->i = temp->prev->i;
        temp->j = temp->prev->j;
        temp->val = temp->prev->val;
        }
        belly->head->i = new_i;
        belly->head->j = new_j;
        belly->head->val = PIXEL_ADDRESS(I, new_j, k);
    }

    else{
        new_i = belly->head->i;
        new_j = belly->head->j;

        switch (belly->direction){
        case UP:    new_j++; break;
        case DOWN:  new_j--; break;
        case LEFT:  new_i--; break;
        case RIGHT: new_i++; break;
        }

        if(new_i>48){new_i=1;}
        else if(new_i == 0){new_i=48;}

        if(new_j>32){new_j=1;}
        else if(new_j == 0){new_j=32;}

        uint16_t k = CURRENT_PAD(new_i);
        uint16_t I = LOCAL_I(new_i, k);

        if(!ruleCheck(belly, PIXEL_ADDRESS(I, new_j, k))){
            belly->end = true;
            return;
        }
        for(temp= belly->tail; temp->prev != NULL; temp = temp->prev){
        temp->i = temp->prev->i;
        temp->j = temp->prev->j;
        temp->val = temp->prev->val;
        }
        belly->head->i = new_i;
        belly->head->j = new_j;
        belly->head->val = PIXEL_ADDRESS(I, new_j, k);
    }
    //sort(belly);
    eatFood = false;
    displayGrid(belly, food);

}

void initSnake(SnakeBelly *belly, struct Cell *food){

    uint8_t x = FIRST_PIXEL_X;
    uint8_t y = FIRST_PIXEL_Y;
    uint8_t z = CURRENT_PAD(x);
    uint8_t i_ = LOCAL_I(x, z);

    for(uint8_t i=0; i<SNAKE_SIZE_INIT; i++){
        x--;
        z = CURRENT_PAD(x);
        i_ = LOCAL_I(x, z);
        push(belly, PIXEL_ADDRESS(i_, y, z), x, y, false);
    }
    displayGrid(belly, food);
}

/*============================================================================================*
 * RULES — collisions, growth, scoring, bounds, difficulty                                    *
 *============================================================================================*/
bool ruleCheck(SnakeBelly *belly, uint16_t food){
    struct Cell *temp;

    for(temp = belly->head; temp != NULL; temp = temp->next){
        if(food == temp->val){
            return false;
        }
    }
    return true;
}

 bool foodCheck(SnakeBelly *belly, struct Cell *food, uint16_t address, bool *eatFood){
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint16_t val;

    char c[30];
    if(address == food->val){
        *eatFood = true;
        push(belly, food->val, food->i, food->j, food->poison);
        sprintf(c, "Belly Count: %d\n", belly->count);

        bool stop = false;
        while(!stop){
            struct Cell *temp;
            x = (rand() % 48) + 1;
            y = (rand() % 32) + 1;
            z = CURRENT_PAD(x);
            val = PIXEL_ADDRESS(x, y, z);

            for(temp = belly->head; temp != NULL; temp = temp->next){
                if(temp->val == val){
                    stop = false;
                    break;
                }
                stop = true;
            }
        }

        food->i = x;
        food->j = y;
        food->val = PIXEL_ADDRESS(LOCAL_I(x, z), y, z);
        return !food->poison;
    }
    return true;
 }

/*============================================================================================*
 * EFFECTS — sound, LED flashes, animations                                                    *
 *============================================================================================*/

 void gameInit(SnakeBelly *belly, struct Cell *food){

    /*Square coil*/
    uint8_t x = FIRST_PIXEL_X;
    uint8_t y = FIRST_PIXEL_Y;
    uint8_t z = CURRENT_PAD(x);
    uint8_t i_ = LOCAL_I(x, z);
    uint16_t p = 2;

    bool x_left = false;
    bool y_up = false;
    while(p<5){
        
        for(uint8_t i=0; i<p; i++){
            if(!x_left){
                x--;
            }
            else{
                x++;
            }
            z = CURRENT_PAD(x);
            i_ = LOCAL_I(x, z);
            push(belly, PIXEL_ADDRESS(i_, y, z), x, y, false);
            displayGrid(belly, food);
            _delay_ms(50);
        }
        p++;

        for(uint8_t j=0; j<p; j++){
            if(!y_up){
                y--;
            }
            else{
                y++;
            }
            z = CURRENT_PAD(x);
            i_ = LOCAL_I(x, z);
            push(belly, PIXEL_ADDRESS(i_, y, z), x, y, false);
            displayGrid(belly, food);
            _delay_ms(50);
        }
        p++;

        y_up = (!y_up)?true:false;
        x_left = (!x_left)?true:false;

        displayGrid(belly, food);
         _delay_ms(50);
    }

    belly->color = COLOR_RED;

    for(int i=0; i<4; i++){
        displayClear();
        displayGrid(belly, food);
        _delay_ms(300);
    }

    displayClear();
    popAll(belly);
 }

 void gameEnd(SnakeBelly *belly, struct Cell *food){
    belly->color = COLOR_RED;

    for(int i=0; i<10; i++){
        displayClear();
        displayGrid(belly, food);
        _delay_ms(300);
    }

    popAll(belly);
 }

 void initialAnimation(){
    uint32_t color_ = COLOR_CYAN;

    uint8_t x_start = 45;
    uint8_t y_start = 26;

    uint8_t s[9] = {0x7E, 0x40, 0x40, 0x40, 0x7E, 0x02, 0x02, 0x02, 0x7E};
    uint8_t n[9] = {0x42, 0x52, 0x52, 0x4A, 0x46, 0x46, 0x42, 0x42, 0x42};
    uint8_t a[9] = {0x18, 0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42};
    uint8_t k[9] = {0x42, 0x4C, 0x66, 0x70, 0x78, 0x70, 0x66, 0x4C, 0x42};
    uint8_t e[9] = {0x7E, 0x40, 0x40, 0x40, 0x7E, 0x40, 0x40, 0x40, 0x7E};

    draw(s, x_start, y_start, color_);
    x_start -= 8;
    draw(n, x_start, y_start, color_);
    x_start -= 8;
    draw(a, x_start, y_start, color_);
    x_start -= 8;
    draw(k, x_start, y_start, color_);
    x_start -= 8;
    draw(e, x_start, y_start, color_);

    
 }

 void draw(uint8_t *arr, uint8_t x_start, uint8_t y_start, uint32_t color){

    for(int i=0; i<9; i++){
        for(int j=0; j<8; j++){
            if((arr[i])<<j & 0x80){
                ws2812bWrite((color >> 16 & 0xff), (color >> 8 & 0xff), color & 0xff, x_start-j, y_start-i);
            }
            else{
                ws2812bWrite(0x00, 0x00, 0x00, x_start-j, y_start-i);
            }
        }
    }
 }

 /*
0------0 
0-000000
0-000000
0-000000
0------0
000000-0
000000-0
000000-0
0------0

0-0000-0 
0-0-00-0
0-0-00-0
0-00-0-0
0-000--0
0-000--0
0-0000-0
0-0000-0
0-0000-0

000--000
0-0000-0
0-0000-0
0-0000-0
0------0
0-0000-0
0-0000-0
0-0000-0
0-0000-0

0-0000-0
0-000-00
0-00-000
0-0-0000
0--00000
0---0000
0-00-000
0-000-00
0-0000-0

0------0
0-000000
0-000000
0-000000
0------0
0-000000
0-000000
0-000000
0------0
 
 
 */
 #endif