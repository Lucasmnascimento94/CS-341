#include "shared_memory.h"
#include "uart.h"

typedef struct{
    uint32_t score_start;
    uint32_t cmd_start;
    uint32_t stack_start;
    uint32_t node_start;
    uint32_t buffer_start;
}BASES;
BASES bases;

#if (GAME == SNAKE)

void computeBlockSizes(){
    sram_map.score.block_size   = 5 + sizeof(sram_map.score) - sizeof(sram_map.score.block_size); 
    sram_map.cmd.block_size     = 5 + sizeof(sram_map.cmd) - sizeof(sram_map.cmd.block_size);
    sram_map.stack.block_size   = 5 + sizeof(sram_map.stack) - sizeof(sram_map.stack.block_size);
    sram_map.node.block_size    = 5 + sizeof(sram_map.node) - sizeof(sram_map.node.block_size);
    sram_map.buffer.block_size  = 5 + sizeof(sram_map.buffer) - sizeof(sram_map.buffer.block_size) + SCREEN_BUFFER_SIZE;

}

void computeBases(){
    bases.buffer_start    = MEMORY_PADDING; // Add a padding from the NULL pointer
    bases.stack_start     = bases.buffer_start + sram_map.buffer.block_size;
    bases.cmd_start       = bases.stack_start + sram_map.stack.block_size;
    bases.score_start     = bases.cmd_start + sram_map.cmd.block_size;
    bases.node_start      = bases.score_start + sram_map.score.block_size; 
}

void sharedMemoryInit(){
    computeBlockSizes();
    computeBases();
}

void erase(){
    for(uint32_t i=0; i < SRAM_SIZE; i++){
        sramWriteByte(&spi, NULL_PTR, i);
    }
}

void pushNode(struct NODE *node){
    uint32_t bs        = sram_map.node.block_size;
    uint8_t  first     = (sram_map.stack.count == 0);
    uint32_t base      = first ? bases.node_start : sram_map.stack.current_node;
    uint32_t tail = first ? NULL_PTR : sram_map.stack.tail;


    // Capacity guard (choose: wrap, free-list, or error)
    if (base + bs > SRAM_SIZE) {
        // handle OOM or wrap here
        return;
    }
    /*______________header_____________*/
    sramWriteStringPoll(&spi, MAGIC_NODE, NODE_MAGIC(base), 4);    // keep 4 if that's your spec

    /*_____________payload_____________*/
    sramWriteU16(&spi, node->addr, NODE_ADDR(base));
    sramWriteByte(&spi, node->i, NODE_I(base));
    sramWriteByte(&spi, node->j, NODE_J(base));
    sramWriteByte(&spi, node->g, NODE_G(base));
    sramWriteByte(&spi, node->r, NODE_R(base));
    sramWriteByte(&spi, node->b, NODE_B(base));
    sramWriteByte(&spi, node->opcode, NODE_OPCODE(base));


    /*_____________links (new node)____*/
    sramWriteU32(&spi, tail, NODE_PREV(base));
    node->prev = tail;
    sramWriteU32(&spi, NULL_PTR,  NODE_NEXT(base));
    node->next = NULL_PTR;

    /*_____________link old tail______*/
    if (!first) {
        sramWriteU32(&spi, base, NODE_NEXT(tail));
    } else {
        sram_map.stack.head = base; // first node
    }


    /*_____________commit flag last____*/
    sramWriteByte(&spi, FLAG_VALID, NODE_FLAGS(base));

    /*_____________RAM metadata________*/
    sram_map.stack.count++;
    sram_map.stack.tail = base;
    sram_map.stack.current_node = sram_map.stack.tail + bs;
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

void bufferWrite(uint8_t g, uint8_t r, uint8_t b,  uint16_t index){
    uint32_t addr = index + BUFFER_DATA(bases.buffer_start);
    if(addr > (BUFFER_DATA(bases.buffer_start) + (uint32_t)SCREEN_BUFFER_SIZE)) return;
    sramWriteByte(&spi, 0X00, addr++);
    sramWriteByte(&spi, g, addr++);
    sramWriteByte(&spi, r, addr++);
    sramWriteByte(&spi, b, addr);
}

void bufferRead(uint32_t *data, uint16_t index){
    uint32_t addr = (uint32_t)index + BUFFER_DATA(bases.buffer_start);
    if(addr > (BUFFER_DATA(bases.buffer_start) + SCREEN_BUFFER_SIZE)) return;
    sramReadU32(&spi, data, addr); 
}

void buferClear(){
    uint32_t addr = BUFFER_DATA(bases.buffer_start);
    for(; addr < SCREEN_BUFFER_SIZE; addr++){
        sramWriteByte(&spi, NULL_PTR, addr);
    }
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

void loadCommand(){
    uint32_t base        = bases.cmd_start;

    /*______________header_____________*/
    sramWriteStringPoll(&spi, MAGIC_COMMANDS, COMMANDS_MAGIC(base), 2);    // keep 4 if that's your spec

    /*_____________payload_____________*/
    sramWriteU32(&spi, sram_map.cmd.cmdID,            COMMANDS_CMDID(base));
    sramWriteU16(&spi, sram_map.cmd.arg1,             COMMANDS_ARGS1(base));
    sramWriteU16(&spi, sram_map.cmd.arg2,             COMMANDS_ARGS2(base));
    sramWriteU16(&spi, sram_map.cmd.arg3,             COMMANDS_ARGS3(base));

    /*_____________commit flag last____*/
    sramWriteByte(&spi, FLAG_VALID, COMMANDS_FLAGS(base));
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

void readNode(struct NODE *node, uint32_t base){
    memset(node, 0, sizeof(struct NODE));
    char magic[5];
    magic[5] = '\0';
    sramReadString(&spi, magic, 2, NODE_MAGIC(base));
    if(strcmp(magic, MAGIC_NODE) != 0){
        uartWrite_("Error... <Invalid Header> \n");
        uartWrite_(magic);
    }
    sramReadU32(&spi, &node->next,      NODE_NEXT(base));
    sramReadU32(&spi, &node->prev,      NODE_PREV(base));
    sramReadU16(&spi, &node->addr,      NODE_ADDR(base));
    sramReadByte(&spi, &node->i,        NODE_I(base));
    sramReadByte(&spi, &node->j,        NODE_J(base));
    sramReadByte(&spi, &node->g,        NODE_G(base));
    sramReadByte(&spi, &node->r,        NODE_R(base));
    sramReadByte(&spi, &node->b,        NODE_B(base));
    sramReadByte(&spi, &node->opcode,   NODE_OPCODE(base));
}

#elif (GAME == SPACE)

#elif (GAME == PONG)

#endif