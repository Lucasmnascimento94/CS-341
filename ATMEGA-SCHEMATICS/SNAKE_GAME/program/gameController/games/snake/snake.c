#include "snake.h"
#include "WS2812B.h"
#include "uart.h"

/*============================================================================================*
 * RENDER — framebuffer/bitset and scanout                                                     *
 *============================================================================================*/
void wrap(uint8_t *i, uint8_t *j){
    if(*i>GRID_W){*i=1;}
    else if(*i == 0){*i=GRID_W;}

    if(*j>GRID_H){*j=1;}
    else if(*j == 0){*j=GRID_H;}
}

void walk(SnakeBelly *belly, struct Cell *food){
    if(belly == NULL || food == NULL) return;

    uint8_t new_i = belly->head.i;
    uint8_t new_j = belly->head.j;
    bool eatFood = false;

    switch (belly->direction){
        case UP:    new_j++; break;
        case DOWN:  new_j--; break;
        case LEFT:  new_i--; break;
        case RIGHT: new_i++; break;
    }
    wrap(&new_i, &new_j);

    uint16_t k = CURRENT_PAD(new_i);
    uint16_t I = LOCAL_I(new_i, k);

    if(!ruleCheck(PIXEL_ADDRESS(I, new_j, k))){
        belly->end = true;
        return;
    }

    if(!foodCheck(PIXEL_ADDRESS(I, new_j, k), &eatFood)){
        belly->end = true;
        return;
    }

    if(!eatFood){ move(belly, new_i, new_j, k, I);}
    else{move_grow(belly, new_i, new_j, k, I);}
    //sort(belly);
    eatFood = false;
    //displayGrid(belly, food);

}

void move(SnakeBelly *belly, uint8_t new_i, uint8_t new_j, uint8_t k, uint8_t I){
    uint32_t tail_base = sram_map.cell.tail_base;
    uint32_t current_addr = sram_map.cell.tail_base;
    struct Cell prev_cell;
    struct Cell current_cell;

    readCell(&current_cell, tail_base);
    for(;current_cell.prev_addr != NULL_PTR; readCell(&prev_cell, CELL_PREV(current_addr))){
        
        current_cell.i = prev_cell.i;
        current_cell.j = prev_cell.j;
        current_cell.val = prev_cell.val;
        updateCell(&current_cell, current_addr);
        current_addr = current_cell.prev_addr;
        current_cell = prev_cell;
    }

    readCell(&current_cell, sram_map.cell.head_base);
    current_cell.i = new_i;
    current_cell.j = new_j;
    current_cell.val = PIXEL_ADDRESS(I, new_j, k);
    updateCell(&current_cell, sram_map.cell.head_base);
}

void move_grow(SnakeBelly *belly, uint8_t new_i, uint8_t new_j, uint8_t k, uint8_t I) {
    struct Cell head_cell;
    struct Cell current_cell;
    struct Cell prev_cell;
    uint32_t current_addr;

    // ===== 1️⃣ Read HEAD from SRAM =====
    readCell(&head_cell, sram_map.cell.head_base);

    // ===== 2️⃣ Calculate new position (exactly as before) =====
    new_i = head_cell.i;
    new_j = head_cell.j;

    switch (belly->direction) {
        case UP:    new_j++; break;
        case DOWN:  new_j--; break;
        case LEFT:  new_i--; break;
        case RIGHT: new_i++; break;
    }

    // ===== 3️⃣ Wrap around edges =====
    if (new_i > GRID_W) new_i = 1;
    else if (new_i == 0) new_i = GRID_W;

    if (new_j > GRID_H) new_j = 1;
    else if (new_j == 0) new_j = GRID_H;

    // ===== 4️⃣ Compute pad and local index =====
    k = CURRENT_PAD(new_i);
    I = LOCAL_I(new_i, k);

    // ===== 5️⃣ Collision / rule check =====
    if (!ruleCheck(PIXEL_ADDRESS(I, new_j, k))) {
        belly->end = true;
        return;
    }

    // ===== 6️⃣ Move body: tail → head =====
    current_addr = sram_map.cell.tail_base;
    readCell(&current_cell, current_addr);

    while (current_cell.prev_addr != NULL_PTR) {
        readCell(&prev_cell, current_cell.prev_addr);

        current_cell.i   = prev_cell.i;
        current_cell.j   = prev_cell.j;
        current_cell.val = prev_cell.val;

        updateCell(&current_cell, current_addr);

        current_addr = current_cell.prev_addr;
        current_cell = prev_cell;
    }

    // ===== 7️⃣ Update head with new position =====
    head_cell.i   = new_i;
    head_cell.j   = new_j;
    head_cell.val = PIXEL_ADDRESS(I, new_j, k);

    updateCell(&head_cell, sram_map.cell.head_base);
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
        //push(belly, PIXEL_ADDRESS(i_, y, z), x, y, false);
    }
    //displayGrid(belly, food);
}

/*============================================================================================*
 * RULES — collisions, growth, scoring, bounds, difficulty                                    *
 *============================================================================================*/
bool ruleCheck(uint16_t food_addr){
    struct Cell cur;
    uint32_t addr = sram_map.cell.head_base;

    while (addr != NULL_PTR) {
        readCell(&cur, addr);
        if (food_addr == cur.val) {
            return false;  // collision
        }
        addr = cur.next_addr;
    }
    return true; // no match anywhere
}

bool foodCheck(uint16_t address, bool *eatFood) {
    struct Cell food;
    struct Cell belly_cell;
    uint8_t x, y, z;
    uint16_t val;

    // ===== 1️⃣ Read the FOOD cell from SRAM =====
    readCell(&food, sram_map.cell.food_base);

    // ===== 2️⃣ Check if the snake's head reached the food =====
    if (address == food.val) {
        *eatFood = true;

        // Push new cell for growth
        pushCell(&food);

        // ===== 3️⃣ Generate new food coordinates =====
        bool stop = false;
        while (!stop) {
            x = (rand() % 48) + 1;
            y = (rand() % 32) + 1;
            z = CURRENT_PAD(x);
            val = PIXEL_ADDRESS(x, y, z);

            // Walk the entire snake list in SRAM to ensure no overlap
            uint32_t addr = sram_map.cell.head_base;
            stop = true;
            while (addr != NULL_PTR) {
                readCell(&belly_cell, addr);
                if (belly_cell.val == val) {
                    stop = false; // overlap — regenerate
                    break;
                }
                addr = belly_cell.next_addr;
            }
        }

        // ===== 4️⃣ Store the new food cell back into SRAM =====
        food.i = x;
        food.j = y;
        food.val = PIXEL_ADDRESS(LOCAL_I(x, z), y, z);
        updateCell(&food, sram_map.cell.food_base);

        return !food.poison; // same logic as your original
    }

    // ===== 5️⃣ No food eaten =====
    return true;
}
/*============================================================================================*
 * EFFECTS — sound, LED flashes, animations                                                    *
 *============================================================================================*/

 void gameInit(){
 }

 void gameEnd(){
 }