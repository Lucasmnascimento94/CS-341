#include "shared_memory.h"
#include "uart.h"
#include "string.h"

typedef struct{
    uint32_t score_start;
    uint32_t cmd_start;
    uint32_t stack_start;
    uint32_t node_start;
    uint32_t buffer_start;
    uint32_t food_start;
}BASES;
BASES bases;

#if (GAME == SNAKE)

void computeBlockSizes(){
    sram_map.score.block_size   = 5 + sizeof(sram_map.score) - sizeof(sram_map.score.block_size); 
    sram_map.cmd.block_size     = 5 + sizeof(sram_map.cmd) - sizeof(sram_map.cmd.block_size);
    sram_map.stack.block_size   = 5 + sizeof(sram_map.stack) - sizeof(sram_map.stack.block_size);
    sram_map.node.block_size    = 5 + sizeof(sram_map.node) - sizeof(sram_map.node.block_size);
    sram_map.buffer.block_size  = 5 + sizeof(sram_map.buffer) - sizeof(sram_map.buffer.block_size) + SCREEN_BUFFER_SIZE*3;

}

void computeBases(){
    bases.buffer_start    = MEMORY_PADDING; // Add a padding from the NULL pointer
    bases.stack_start     = bases.buffer_start + sram_map.buffer.block_size;
    bases.cmd_start       = bases.stack_start + sram_map.stack.block_size;
    bases.score_start     = bases.cmd_start + sram_map.cmd.block_size;
    bases.food_start      = bases.score_start + sram_map.score.block_size + MEMORY_PADDING; 
    bases.node_start      = bases.food_start + sram_map.node.block_size; 
}

void initCtaInt(){
    SRAM_CTS_DDR  &= ~(1<<SRAM_CTS_PIN);

    SRAM_RTS_PORT |= (1<<SRAM_RTS_PIN);
    SRAM_RTS_DDR  |= (1<<SRAM_RTS_PIN);
}

void waitRand(uint16_t n){
    for(int i=0; i<n;i++){
        _delay_us(1);
    }
}

void memAcquire(){
    uint16_t k=10;
    char c[15];
    while(1){
        while(!((SRAM_CTS_PIN_PORT >> SRAM_CTS_PIN) & 0x01)); // check availability
        SRAM_RTS_PORT &= ~(1<<SRAM_RTS_PIN); // Acquire memory
        _delay_us(1); // Gieve it a brief

        if((SRAM_CTS_PIN_PORT >> SRAM_CTS_PIN) & 0x01){break;}
        else{
            sprintf(c, "k: %d\n", k);
            uartWrite_(c);
            memFree();
            waitRand((int)((rand() % k++) + 1)); // Gieve it a random brief
        }
    }
}

void memFree(){
    SRAM_RTS_PORT |= (1<<SRAM_RTS_PIN);
}

void sharedMemoryInit(){
    computeBlockSizes();
    computeBases();
    initCtaInt();
    sram_map.sram_cta = false;
}

/*_____ Clear SRAM_____*/
void erase(){
    for(uint32_t i=0; i < SRAM_SIZE; i++){
        sramWriteByte(&spi, NULL_PTR, i);
    }
}


/*________Linked List______*/
void pushNode(struct NODE *node){
    uint32_t bs        = sram_map.node.block_size;
    uint8_t  first     = (sram_map.stack.count == 0);
    uint32_t base      = first ? bases.node_start : sram_map.stack.current_node;
    uint32_t head = first ? NULL_PTR : sram_map.stack.head;


    // Capacity guard (choose: wrap, free-list, or error)
    if (base + bs > SRAM_SIZE) {
        // handle OOM or wrap here
        return;
    }
    /*______________header_____________*/
    sramWriteStringPoll(&spi, MAGIC_NODE, NODE_MAGIC(base), 4);    // keep 4 if that's your spec

    /*_____________payload_____________*/
    sramWriteU32(&spi, node->addr, NODE_ADDR(base));
    sramWriteByte(&spi, node->i, NODE_I(base));
    sramWriteByte(&spi, node->j, NODE_J(base));
    sramWriteByte(&spi, node->g, NODE_G(base));
    sramWriteByte(&spi, node->r, NODE_R(base));
    sramWriteByte(&spi, node->b, NODE_B(base));
    sramWriteByte(&spi, node->opcode, NODE_OPCODE(base));


    /*_____________links (new node)____*/
    sramWriteU32(&spi, head, NODE_NEXT(base));
    node->next = head;
    sramWriteU32(&spi, NULL_PTR,  NODE_PREV(base));
    node->prev = NULL_PTR;

    /*_____________link old tail______*/
    if (!first) {
        sramWriteU32(&spi, base, NODE_PREV(head));
    } else {
        sram_map.stack.tail = base; // first node
    }



    
    /*_____________commit flag last____*/
    sramWriteByte(&spi, FLAG_VALID, NODE_FLAGS(base));

    /*_____________RAM metadata________*/
    sram_map.stack.count++;
    sram_map.stack.head = base;
    sram_map.stack.current_node = sram_map.stack.head + bs;
}

void popNode(){
    if(sram_map.stack.count == 0) return;
    uint32_t bs          = sram_map.node.block_size;
    uint32_t tail        = sram_map.stack.tail;
    uint32_t prev        = NULL_PTR;

    sramWriteByte(&spi, FLAG_DIRTY, NODE_FLAGS(tail));

    /*_____________retrieve next tail addr____*/
    sramReadU32(&spi, &prev, NODE_PREV(tail));

    /*_____________links to NULL____*/
    if (prev != NULL_PTR) {
        // Multi-node list: unlink tail by clearing prev->NEXT
        sramWriteU32(&spi, NULL_PTR, NODE_NEXT(prev));
        sram_map.stack.tail = prev;
    } else {
        // Single-node list: become empty
        sram_map.stack.tail = NULL_PTR;
        sram_map.stack.head = NULL_PTR;
    }
    
    /*_____________Clear Block________*/
    for(uint32_t i=0; i<bs; i++){
        sramWriteByte(&spi, 0x00, tail+i);
    }

    /*_____________RAM metadata________*/
    sram_map.stack.current_node = tail; 
    sram_map.stack.count--;
}

void readNode(struct NODE *node, uint32_t base){
    memset(node, 0, sizeof(struct NODE));
    char c[40];
    char magic[5];
    magic[4] = '\0';
    sramReadString(&spi, (uint8_t *)magic, 4, NODE_MAGIC(base));
    if(strcmp(magic, MAGIC_NODE) != 0){
        uartWrite_("Error... <Invalid Header> \n");
        sprintf(c, "..expected..<%s>..actual..<%s>\n", (char *)MAGIC_NODE, magic);
        uartWrite_(c);
    }
    sramReadU32(&spi, &node->next,      NODE_NEXT(base));
    sramReadU32(&spi, &node->prev,      NODE_PREV(base));
    sramReadU32(&spi, &node->addr,      NODE_ADDR(base));
    sramReadByte(&spi, &node->i,        NODE_I(base));
    sramReadByte(&spi, &node->j,        NODE_J(base));
    sramReadByte(&spi, &node->g,        NODE_G(base));
    sramReadByte(&spi, &node->r,        NODE_R(base));
    sramReadByte(&spi, &node->b,        NODE_B(base));
    sramReadByte(&spi, &node->opcode,   NODE_OPCODE(base));
}

void updateNode(struct NODE *node, uint32_t base){
    sramWriteU32(&spi, node->next,      NODE_NEXT(base));
    sramWriteU32(&spi, node->prev,      NODE_PREV(base));
    sramWriteU32(&spi, node->addr,      NODE_ADDR(base));
    sramWriteByte(&spi, node->i,        NODE_I(base));
    sramWriteByte(&spi, node->j,        NODE_J(base));
    sramWriteByte(&spi, node->g,        NODE_G(base));
    sramWriteByte(&spi, node->r,        NODE_R(base));
    sramWriteByte(&spi, node->b,        NODE_B(base));
    sramWriteByte(&spi, node->opcode,   NODE_OPCODE(base));
}

void loadFood(struct NODE *food){
    static bool init = true;
    uint32_t base = bases.food_start;


    if(init){
        sramWriteStringPoll(&spi, MAGIC_NODE, NODE_MAGIC(base), 4);
        init = false;
        sram_map.stack.food = base;
    }
    
    sramWriteU32(&spi, food->addr, NODE_ADDR(base));
    sramWriteByte(&spi, food->i, NODE_I(base));
    sramWriteByte(&spi, food->j, NODE_J(base));
    sramWriteByte(&spi, food->g, NODE_G(base));
    sramWriteByte(&spi, food->r, NODE_R(base));
    sramWriteByte(&spi, food->b, NODE_B(base));
    sramWriteByte(&spi, food->opcode, NODE_OPCODE(base));
    bufferWrite(food->g, food->r, food->b, food->addr);
}

void loadStack(){
    uint32_t base = bases.stack_start;
    /*______________header_____________*/
    sramWriteStringPoll(&spi, MAGIC_STACK, STACK_MAGIC(bases.stack_start), 4);    // keep 4 if that's your spec

    /*_____________payload_____________*/
    sramWriteU16(&spi, sram_map.stack.count,        STACK_COUNT(base));

    /*_____________links____*/
    sramWriteU32(&spi, NULL_PTR, STACK_CURRENT_NODE(base));
    sramWriteU32(&spi, NULL_PTR, STACK_HEAD(base));
    sramWriteU32(&spi, NULL_PTR, STACK_TAIL(base));
    sramWriteU32(&spi, NULL_PTR, STACK_FOOD(base));

    /*_____________commit flag last____*/
    sramWriteByte(&spi, FLAG_VALID, STACK_FLAGS(base));

}

void loadBufferFromStack(){
    uint32_t addr_head = sram_map.stack.head;
    uint32_t count = sram_map.stack.count;
    if(count == 0) return;

    struct NODE node;
    readNode(&node, addr_head);
    for(;node.next != NULL_PTR; readNode(&node, node.next)){
        bufferWrite(node.g, node.r, node.b, node.addr);

       // sprintf(c, "NODE: i:%d | j: %d | addr: %ld | NEXT_ADDR: %lx | PREV_ADDR: %lx\n", node.i, node.j, node.addr, node.next, node.prev);
       // uartWrite_(c);
       // sprintf(c, "pushing: g_: %x | r_: %x | b_: %x\n", node.g, node.r, node.b);
       // uartWrite_(c);
    }
}

/*________Buffer______*/
void bufferWrite(uint8_t g, uint8_t r, uint8_t b,  uint32_t index){

    uint32_t addr = index*3 + BUFFER_DATA(bases.buffer_start);
    if(addr > (BUFFER_DATA(bases.buffer_start) + (uint32_t)sram_map.buffer.block_size)) return;
    sramWriteByte(&spi, g, addr++);
    sramWriteByte(&spi, r, addr++);
    sramWriteByte(&spi, b, addr);
}


void bufferRead(){
    uint32_t addr = BUFFER_DATA(bases.buffer_start);
    sramReadBuffer(&spi, (uint32_t)SCREEN_BUFFER_SIZE, addr);
}

void bufferClear(){
    for(int i=0; i<SCREEN_BUFFER_SIZE*3; i++){
        bufferWrite(0, 0, 0, i);
    }

}

void buferClear(){
    uint32_t addr = BUFFER_DATA(bases.buffer_start);
    for(; addr < SCREEN_BUFFER_SIZE; addr++){
        sramWriteByte(&spi, NULL_PTR, addr);
    }
}


/*________Commands Parsing______*/
void loadCommand(){
    uint32_t base        = bases.cmd_start;

    /*______________header_____________*/
    sramWriteStringPoll(&spi, MAGIC_COMMANDS, COMMANDS_MAGIC(base), 4
);    // keep 4 if that's your spec

    /*_____________payload_____________*/
    sramWriteU32(&spi, sram_map.cmd.cmdID,            COMMANDS_CMDID(base));
    sramWriteU16(&spi, sram_map.cmd.arg1,             COMMANDS_ARGS1(base));
    sramWriteU16(&spi, sram_map.cmd.arg2,             COMMANDS_ARGS2(base));
    sramWriteU16(&spi, sram_map.cmd.arg3,             COMMANDS_ARGS3(base));

    /*_____________commit flag last____*/
    sramWriteByte(&spi, FLAG_VALID, COMMANDS_FLAGS(base));
}

void getCommand(){
    uint32_t base        = bases.cmd_start;

    /*_____________read Commands and Arguments_____________*/
    sramReadU32(&spi, &sram_map.cmd.cmdID, COMMANDS_CMDID(base));
    sramReadU16(&spi, &sram_map.cmd.arg1, COMMANDS_ARGS1(base));
    sramReadU16(&spi, &sram_map.cmd.arg2, COMMANDS_ARGS2(base));
    sramReadU16(&spi, &sram_map.cmd.arg3, COMMANDS_ARGS3(base));
}

void loadScore(){
    uint32_t base = bases.score_start;
    /*______________header_____________*/
    sramWriteStringPoll(&spi, MAGIC_SCORE, SCORE_MAGIC(base), 4);    // keep 4 if that's your spec

    /*_____________payload_____________*/
    sramWriteU16(&spi, sram_map.score.record_score,          SCORE_HRECORD(base));
    sramWriteU16(&spi, sram_map.score.current_score,         SCORE_CRECORD(base));
    if(sram_map.score.player_name != NULL)
    sramWriteStringPoll(&spi, (char *)sram_map.score.player_name,    SCORE_PNAME(base), 12); 
    if(sram_map.score.game_name != NULL) 
    sramWriteStringPoll(&spi, (char *)sram_map.score.game_name,      SCORE_GNAME(base), 12);

    /*_____________commit flag last____*/
    sramWriteByte(&spi, FLAG_VALID, SCORE_FLAGS(base));
}

void getScore(){
    static bool nameRead = false;
    uint32_t base = bases.score_start;

    /*_____________payload_____________*/
    sramReadU16(&spi, &sram_map.score.record_score, SCORE_HRECORD(base));
    sramReadU16(&spi, &sram_map.score.current_score, SCORE_CRECORD(base));
    if(!nameRead){
        sramReadString(&spi, sram_map.score.player_name, 12, SCORE_PNAME(base)); 
        sramReadString(&spi, sram_map.score.game_name, 12, SCORE_GNAME(base));
        nameRead = true;
    }
}
#elif (GAME == SPACE)

#elif (GAME == PONG)

#endif