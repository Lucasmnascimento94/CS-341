#include "snake.h"
#include "shared_memory.h"
#include "WS2812B.h"
#include "uart.h"

#if (GAME == SNAKE)
/*============================================================================================*
 * RENDER — framebuffer/bitset and scanout                                                     *
 *============================================================================================*/

struct NODE node;
struct STACK stack;

void walk(){
    getCommand();

    struct NODE node;
    readNode(&node, sram_map.stack.tail);
    uint32_t old_tail_pixel = node.addr;

    uint8_t cmd = sram_map.cmd.cmdID;
    struct NODE prev;
    readNode(&node, sram_map.stack.head);
    uint8_t new_i = node.i;
    uint8_t new_j = node.j;
    bool poison = false;

    switch (cmd){
        case WALK_UP:    new_j++; break;
        case WALK_DOWN:  new_j--; break;
        case WALK_RIGHT:  new_i--; break;
        case WALK_LEFT: new_i++; break;
    }

    if(new_i==SCREEN_WIDTH){new_i=0;}
    else if(new_i == (uint8_t)-1){new_i=SCREEN_WIDTH-1;}
    if(new_j==SCREEN_HEIGHT){new_j=0;}
    else if(new_j == (uint8_t)-1){new_j=SCREEN_HEIGHT-1;}

    if(collisionCheck(PIXEL_ADDRESS(new_i, new_j))){uartWrite_("collision detected\n"); return;}
    bool eat = foodCheck(PIXEL_ADDRESS(new_i, new_j), &poison);
    if(poison){uartWrite_("Poison food detected\n"); return;}
    
    if(1){
        for(readNode(&node, sram_map.stack.tail);node.prev != NULL_PTR; readNode(&node, node.prev)){
            readNode(&prev, node.prev);
            node.i = prev.i;
            node.j = prev.j;
            node.addr = prev.addr;
            updateNode(&node, prev.next);
        }

        readNode(&node, sram_map.stack.head);
        node.i = new_i;
        node.j = new_j;
        node.addr = PIXEL_ADDRESS(new_i, new_j);
        updateNode(&node, sram_map.stack.head);
    }

    else{
        readNode(&node, sram_map.stack.head);
        new_i = node.i;
        new_j = node.j;

        switch (cmd){
        case WALK_UP:    new_j++; break;
        case WALK_DOWN:  new_j--; break;
        case WALK_RIGHT:  new_i--; break;
        case WALK_LEFT: new_i++; break;
        }

        if(new_i>SCREEN_WIDTH){new_i=1;}
        else if(new_i == 0){new_i=SCREEN_WIDTH;}

        if(new_j>SCREEN_HEIGHT){new_j=1;}
        else if(new_j == 0){new_j=SCREEN_HEIGHT;}

        if(collisionCheck(PIXEL_ADDRESS(new_i, new_j)))return;
        
        readNode(&node, sram_map.stack.tail);
        for(;node.prev != NULL_PTR; readNode(&node, node.prev)){
            readNode(&prev, node.prev);
            node.i = prev.i;
            node.j = prev.j;
            node.addr = prev.addr;
        }
        readNode(&node, sram_map.stack.head);
        node.i = new_i;
        node.j = new_j;
        node.addr = PIXEL_ADDRESS(new_i, new_j);
        updateNode(&node, sram_map.stack.head);
    }
    bufferWrite(0x00, 0x00, 0x00, old_tail_pixel);
    loadBufferFromStack();
}

void initSnake(){
    displayClear();
    uint8_t x = FIRST_PIXEL_X;
    node.g = (SNAKE_COLOR >> 16) & 0XFF;
    node.r = (SNAKE_COLOR >> 8) & 0XFF;
    node.b = (SNAKE_COLOR) & 0XFF;
    node.j = FIRST_PIXEL_Y;
    node.opcode = SPECIAL_2;

    for(uint8_t i=0; i<SNAKE_SIZE_INIT; i++){
        node.addr = (uint32_t) (PIXEL_ADDRESS(x, FIRST_PIXEL_Y));
        node.i = x;
        pushNode(&node);
        x--;
    }
    loadBufferFromStack();
    displayGrid();
}

/*============================================================================================*
 * RULES — collisions, growth, scoring, bounds, difficulty                                    *
 *============================================================================================*/
bool collisionCheck(uint32_t new_node_addr){
    struct NODE temp;
    readNode(&temp, sram_map.stack.head);
    for(; temp.next!=NULL_PTR; readNode(&temp, temp.next)){
        if(new_node_addr == temp.addr){
            sram_map.cmd.cmdID = END_GAME;
            sram_map.cmd.arg1 = ARG_COLLISION;
            return true;
        }
    }
    return false;
}

bool foodCheck(uint32_t new_node_addr, bool *poison){
    struct NODE food;
    struct NODE temp;
    readNode(&food, sram_map.stack.food);
    readNode(&temp, sram_map.stack.head);
    for(;temp.next != NULL_PTR; readNode(&temp, temp.next)){
        if(new_node_addr != food.addr) return false;
    }

    food.g = temp.g;
    food.r = temp.r;
    food.b = temp.b;
    food.opcode = temp.opcode;
    pushNode(&food);
    sram_map.score.current_score++;
    generateFood();

    if(food.opcode == FOOD_POISON){
        sram_map.cmd.cmdID = END_GAME;
        sram_map.cmd.arg1 = FOOD_POISON;
        *poison = true;
    }
    return true;
 }

 void generateFood(){
    struct NODE temp;
    struct NODE food;
    uint8_t x;
    uint8_t y;
    uint32_t val;
    bool stop = false;
    readNode(&food, sram_map.stack.food);
    while(!stop){
        readNode(&temp, sram_map.stack.head);
        x = (rand() % SCREEN_WIDTH) + 1;
        y = (rand() % SCREEN_HEIGHT) + 1;
        val = PIXEL_ADDRESS(x, y);

        for(;temp.next != NULL_PTR; readNode(&temp, temp.next)){
            if(temp.addr == val){
                stop = false;
                break;
            }
            stop = true;
        }
    }

    food.i = x;
    food.j = y;
    food.g = (FOOD_COLOR >> 16) & 0xff;
    food.r = (FOOD_COLOR >> 8) & 0xff;
    food.b = (FOOD_COLOR ) & 0xff;
    food.addr = PIXEL_ADDRESS(x, y);
    loadFood(&food);
 }

/*============================================================================================*
 * EFFECTS — sound, LED flashes, animations                                                    *
 *============================================================================================*/


 void gameEnd(SnakeBelly *belly, struct Cell *food){
    belly->color = COLOR_RED;

    for(int i=0; i<10; i++){
        displayClear();
        displayGrid(belly, food);
        _delay_ms(300);
    }

    popAll(belly);
 }

  void draw(uint8_t *arr, uint8_t x_start, uint8_t y_start, uint32_t color){

    for(int i=0; i<6; i++){
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

 void initialAnimation(){
    uint32_t color_ = COLOR_CYAN;

    uint8_t x_start = 45;
    uint8_t y_start = 21;

   /*uint8_t s[9] = {0x7E, 0x40, 0x40, 0x40, 0x7E, 0x02, 0x02, 0x02, 0x7E};
    uint8_t n[9] = {0x42, 0x42, 0x62, 0x52, 0x4A, 0x46, 0x42, 0x42, 0x42};
    uint8_t a[9] = {0x3C, 0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42};
    uint8_t k[9] = {0x42, 0x44, 0x48, 0x50, 0x60, 0x70, 0x48, 0x44, 0x42};
    uint8_t e[9] = {0x7E, 0x40, 0x40, 0x40, 0x7E, 0x40, 0x40, 0x40, 0x7E};

    draw(s, x_start, y_start, color_);
    x_start -= 8;
    draw(n, x_start, y_start, color_);
    x_start -= 8;
    draw(a, x_start, y_start, color_);
    x_start -= 8;
    draw(k, x_start, y_start, color_);
    x_start -= 8;
    draw(e, x_start, y_start, color_);*/

    uint8_t pac[6] = {0x78, 0xFC, 0xEC, 0xF8, 0xFC, 0x78};
    draw(pac, x_start, y_start, color_);

    
 }
 #endif