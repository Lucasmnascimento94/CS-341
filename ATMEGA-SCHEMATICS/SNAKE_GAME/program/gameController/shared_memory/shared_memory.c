#include "shared_memory.h"
#include "uart.h"

void loadInitials(){

}

void pushCell(struct Cell *cell){
    char c[50];
    uint32_t bs        = sram_map.cell.block_size;
    uint8_t  first     = (sram_map.cell.count == 0);
    uint32_t base      = first ? CELL_START_BASE : sram_map.cell.current_base;
    uint32_t tail_base = first ? NULL_PTR : sram_map.cell.tail_base;


    // Capacity guard (choose: wrap, free-list, or error)
    if (base + bs > SRAM_SIZE) {
        // handle OOM or wrap here
        return;
    }
    /*______________header_____________*/
    sramWriteStringPoll(&spi, MAGIC_CELL, CELL_MAGIC(base), 2);    // keep 4 if that's your spec

    /*_____________payload_____________*/
    sramWriteU16(&spi, cell->val, CELL_VAL(base));
    sramWriteByte(&spi, cell->i, CELL_I(base));
    sramWriteByte(&spi, cell->j, CELL_J(base));
    sramWriteByte(&spi, cell->poison,CELL_POISON(base));


    /*_____________links (new node)____*/
    sramWriteU32(&spi, tail_base, CELL_PREV(base));
    cell->prev_addr = tail_base;
    sramWriteU32(&spi, NULL_PTR,  CELL_NEXT(base));
    cell->next_addr = NULL_PTR;

    /*_____________link old tail______*/
    if (!first) {
        sramWriteU32(&spi, base, CELL_NEXT(tail_base));
    } else {
        sram_map.cell.head_base = base; // first node
    }


    /*_____________commit flag last____*/
    sramWriteByte(&spi, FLAG_VALID, CELL_FLAGS(base));

    /*_____________RAM metadata________*/
    sram_map.cell.count++;
    sram_map.cell.tail_base = base;
    sram_map.cell.current_base = sram_map.cell.tail_base + bs;

    /*sprintf(c, "VAL: %d\n", cell->val);
    uartWrite_(c);
    sprintf(c, "MAGIC_ADDR: %lX\n", CELL_MAGIC(base));
    uartWrite_(c);
    sprintf(c, "FLAG_ADDR: %lX\n", CELL_FLAGS(base));
    uartWrite_(c);
    sprintf(c, "NEXT_ADDR: %lX\n", CELL_NEXT(base));
    uartWrite_(c);
    sprintf(c, "NEXT_VAL: %lX\n", cell->next_addr);
    uartWrite_(c);
    sprintf(c, "PREV_ADDR: %lX\n", CELL_PREV(base));
    uartWrite_(c);
    sprintf(c, "PREV_VAL: %lX\n", cell->prev_addr);
    uartWrite_(c);
    sprintf(c, "VAL_ADDR: %lX\n", CELL_VAL(base));
    uartWrite_(c);
    sprintf(c, "I_ADDR: %lX\n", CELL_I(base));
    uartWrite_(c);
    sprintf(c, "J_ADDR: %lX\n", CELL_J(base));
    uartWrite_(c);
    sprintf(c, "POS_ADDR: %lX\n", CELL_POISON(base));
    uartWrite_(c);
    sprintf(c, "CURRENT_ADDR: %lX\n", sram_map.cell.current_base);
    uartWrite_(c);
    sprintf(c, "TAIL_ADDR: %lX\n", sram_map.cell.tail_base);
    uartWrite_(c);
    sprintf(c, "HEAD_ADDR: %lX\n", sram_map.cell.head_base);
    uartWrite_(c);
    uartWrite_("*******************\n\n");*/
}

void popCell(){
    if(sram_map.cell.count == 0) return;
    uint32_t bs          = sram_map.cell.block_size;
    uint32_t tail_base        = sram_map.cell.tail_base;
    uint32_t prev_base  = NULL_PTR;

    sramWriteByte(&spi, FLAG_DIRTY, CELL_FLAGS(tail_base));

    /*_____________retrieve next tail addr____*/
    sramReadU32(&spi, &prev_base, CELL_PREV(tail_base));

    /*_____________links to NULL____*/
    if (prev_base != NULL_PTR) {
        // Multi-node list: unlink tail by clearing prev->NEXT
        sramWriteU32(&spi, NULL_PTR, CELL_NEXT(prev_base));
        sram_map.cell.tail_base = prev_base;
    } else {
        // Single-node list: become empty
        sram_map.cell.tail_base = NULL_PTR;
        sram_map.cell.head_base = NULL_PTR;
    }
    
    /*_____________Clear Block________*/
    for(uint32_t i=0; i<bs; i++){
        sramWriteByte(&spi, 0x00, tail_base+i);
    }

    /*_____________RAM metadata________*/
    sram_map.cell.current_base = tail_base; 
    sram_map.cell.count--;
}

void loadBelly(SnakeBelly *belly){
    bool init = sram_map.belly.init;
    //uint32_t bs        = sram_map.belly.block_size;

    /*______________header_____________*/
    if(init) sramWriteStringPoll(&spi, MAGIC_BELLY, SNAKE_BELLY_MAGIC(BELLY_BASE), 2);    // keep 4 if that's your spec

    /*_____________payload_____________*/
    sramWriteU16(&spi, belly->count,            SNAKE_BELLY_COUNT(BELLY_BASE));
    sramWriteByte(&spi, belly->direction,       SNAKE_BELLY_DIRECTION(BELLY_BASE));
    sramWriteByte(&spi, belly->walk,            SNAKE_BELLY_WALK(BELLY_BASE));
    sramWriteU32(&spi, belly->color,            SNAKE_BELLY_COLOR(BELLY_BASE));
    sramWriteU32(&spi, belly->color_food,      SNAKE_BELLY_COLOR_FOOD(BELLY_BASE));
    sramWriteByte(&spi, belly->pt,              SNAKE_BELLY_PT(BELLY_BASE));
    sramWriteByte(&spi, belly->end,             SNAKE_BELLY_END(BELLY_BASE));
    sramWriteByte(&spi, belly->begin,           SNAKE_BELLY_BEGIN(BELLY_BASE));

    /*_____________links____*/
    sramWriteU32(&spi, sram_map.cell.head_base, SNAKE_BELLY_HEAD(BELLY_BASE));
    sramWriteU32(&spi, sram_map.cell.tail_base, SNAKE_BELLY_TAIL(BELLY_BASE));
    sramWriteU32(&spi, NULL_PTR,                SNAKE_BELLY_FOOD(BELLY_BASE)); // No food by default

    /*_____________commit flag last____*/
    sramWriteByte(&spi, FLAG_VALID, SNAKE_BELLY_FLAGS(BELLY_BASE));

    /*_____________RAM metadata________*/
    if(init) sram_map.belly.init = false;
    sram_map.belly.flags = FLAG_VALID;
}

void loadMail(){
    //uint32_t bs        = sram_map.mail.block_size;

    /*______________header_____________*/
    sramWriteStringPoll(&spi, MAGIC_MAIL, MAIL_MAGIC(MAIL_BASE), 2);    // keep 4 if that's your spec

    /*_____________payload_____________*/
    sramWriteU32(&spi, sram_map.mail.cmdID,            MAIL_CMDID(MAIL_BASE));
    sramWriteU16(&spi, sram_map.mail.arg1,            MAIL_ARGS1(MAIL_BASE));
    sramWriteU16(&spi, sram_map.mail.arg2,      MAIL_ARGS2(MAIL_BASE));
    sramWriteU16(&spi, sram_map.mail.arg3,              MAIL_ARGS3(MAIL_BASE));

    /*_____________commit flag last____*/
    sramWriteByte(&spi, FLAG_VALID, MAIL_FLAGS(MAIL_BASE));
}

void loadScore(){
    /*______________header_____________*/
    sramWriteStringPoll(&spi, MAGIC_SCORE, SCORE_MAGIC(SCORE_BASE), 2);    // keep 4 if that's your spec

    /*_____________payload_____________*/
    sramWriteU16(&spi, sram_map.score.record_score,          SCORE_HRECORD(SCORE_BASE));
    sramWriteU16(&spi, sram_map.score.current_score,         SCORE_CRECORD(SCORE_BASE));
    sramWriteStringPoll(&spi, (char *)sram_map.score.player_name,    SCORE_PNAME(SCORE_BASE), 12);  
    sramWriteStringPoll(&spi, (char *)sram_map.score.game_name,      SCORE_GNAME(SCORE_BASE), 12);

    /*_____________commit flag last____*/
    sramWriteByte(&spi, FLAG_VALID, SCORE_FLAGS(SCORE_BASE));
}

void updateCell(struct Cell *cell, uint32_t base){
}
void readCell(struct Cell *cell, uint32_t base){
    memset(cell, 0, sizeof(Cell));
    uint8_t magic[3];
    magic[2] = '\0';
    sramReadString(&spi, magic, 2, CELL_MAGIC(base));
    if(strcmp(magic, MAGIC_CELL) != 0){
        uartWrite_("STRINGS NOT EQUAl\n");
        uartWrite_(magic);
    }
    sramReadU32(&spi, &cell->next_addr, CELL_NEXT(base));
    sramReadU32(&spi, &cell->prev_addr, CELL_PREV(base));
    sramReadU16(&spi, &cell->val, CELL_VAL(base));
    sramReadByte(&spi, &cell->i, CELL_I(base));
    sramReadByte(&spi, &cell->j, CELL_J(base));
    sramReadByte(&spi, &cell->poison, CELL_POISON(base));
}
