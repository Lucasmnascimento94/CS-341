#ifndef SHARED_H
#define SHARED_H

#include "includes.h"
#include "string.h"
#include "stdio.h"
#include "spi.h"
#include "sram.h"

#define SRAM_CTA_PORT PORTC
#define SRAM_CTA_DDR  DDRC
#define SRAM_CTA_PIN  PC3

#define SNAKE 1
#define SPACE 2
#define PONG 3
#define PACMAN 4
#define GAME SNAKE

/*________Constant PreDefined Values______*/
#define PAGE_SIZE                   32
#define NULL_PTR                    (uint32_t)0x00
#define FLAG_DIRTY                  0x00u
#define FLAG_VALID                  0xA5u
#define SRAM_SIZE                   0x1FFFF
#define SCREEN_WIDTH                48
#define SCREEN_HEIGHT               32
#define SCREEN_BUFFER_SIZE          (SCREEN_WIDTH*SCREEN_HEIGHT)

/*________Block Sizes______*/

#if (GAME == SNAKE)
/*_______________Header Names______________*/
#define MAGIC_STACK         "STCK"
#define MAGIC_NODE          "NODE"
#define MAGIC_SCORE         "SCOR"
#define MAGIC_BUFFER        "BUFF"
#define MAGIC_COMMANDS      "COMM"
#define MAGIC_FOOD          "FOOD"

/*__________Base addresses Definition___________*/
#define MEMORY_PADDING     ((uint32_t)0x20u)


#pragma pack(push, 1)
struct BUFFER{
    uint8_t width;
    uint8_t height;
    uint16_t size;
    uint32_t block_size;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct STACK{
    uint32_t current_node;          // address of the current Node
    uint32_t head;                  // address of the current head
    uint32_t tail;                  // address of the current tail
    uint32_t food;                  // address of the current food
    uint16_t count;                 // node count
    uint8_t block_size;             // stack list block size
};
#pragma pack(pop)

#pragma pack(push, 1)
struct COMMANDS{
    uint32_t cmdID;
    uint16_t arg1;
    uint16_t arg2;
    uint16_t arg3;
    uint16_t block_size;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SCORE{
    uint8_t player_name[12];
    uint8_t  game_name[12];
    uint16_t record_score;
    uint16_t current_score;
    uint16_t block_size;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct NODE{
    uint32_t next;
    uint32_t prev;
    uint32_t addr;
    uint8_t i;
    uint8_t j;
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t opcode;
    uint16_t block_size;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SRAM_MAP {
    struct SCORE score;
    struct COMMANDS cmd ;
    struct STACK stack;
    struct NODE node;
    struct BUFFER buffer;
    bool sram_cta;
};
#pragma pack(pop)


/*________offsets______*/
#define BUFFER_MAGIC(BASE)            ((uint32_t)(BASE) + (uint32_t)0x00u)
#define BUFFER_FLAGS(BASE)            ((uint32_t)(BASE) + (uint32_t)0x04u)
#define BUFFER_WIDTH(BASE)            ((uint32_t)(BASE) + (uint32_t)0x05u)
#define BUFFER_HEIGHT(BASE)           ((uint32_t)(BASE) + (uint32_t)0x06u)
#define BUFFER_SIZE(BASE)             ((uint32_t)(BASE) + (uint32_t)0x07u)
#define BUFFER_DATA(BASE)             ((uint32_t)(BASE) + (uint32_t)0x09u)

#define STACK_MAGIC(BASE)             ((uint32_t)(BASE) + (uint32_t)0x00u)
#define STACK_FLAGS(BASE)             ((uint32_t)(BASE) + (uint32_t)0x04u)
#define STACK_CURRENT_NODE(BASE)      ((uint32_t)(BASE) + (uint32_t)0x05u)
#define STACK_HEAD(BASE)              ((uint32_t)(BASE) + (uint32_t)0x09u)
#define STACK_TAIL(BASE)              ((uint32_t)(BASE) + (uint32_t)0x0Du)
#define STACK_FOOD(BASE)              ((uint32_t)(BASE) + (uint32_t)0x11u)
#define STACK_COUNT(BASE)             ((uint32_t)(BASE) + (uint32_t)0x15u)

#define NODE_MAGIC(BASE)              ((uint32_t)(BASE) + (uint32_t)0x00u)
#define NODE_FLAGS(BASE)              ((uint32_t)(BASE) + (uint32_t)0x04u)
#define NODE_NEXT(BASE)               ((uint32_t)(BASE) + (uint32_t)0x05u)
#define NODE_PREV(BASE)               ((uint32_t)(BASE) + (uint32_t)0x09u)
#define NODE_ADDR(BASE)               ((uint32_t)(BASE) + (uint32_t)0x0Du)
#define NODE_I(BASE)                  ((uint32_t)(BASE) + (uint32_t)0x11u)
#define NODE_J(BASE)                  ((uint32_t)(BASE) + (uint32_t)0x12u)
#define NODE_G(BASE)                  ((uint32_t)(BASE) + (uint32_t)0x13u)
#define NODE_R(BASE)                  ((uint32_t)(BASE) + (uint32_t)0x14u)
#define NODE_B(BASE)                  ((uint32_t)(BASE) + (uint32_t)0x15u)
#define NODE_OPCODE(BASE)             ((uint32_t)(BASE) + (uint32_t)0x16u)


#define COMMANDS_MAGIC(BASE)          ((uint32_t)(BASE)  + (uint32_t)0X00u)
#define COMMANDS_FLAGS(BASE)          ((uint32_t)(BASE)  + (uint32_t)0X04u)
#define COMMANDS_CMDID(BASE)          ((uint32_t)(BASE)  + (uint32_t)0X05u)
#define COMMANDS_ARGS1(BASE)          ((uint32_t)(BASE)  + (uint32_t)0X09u)
#define COMMANDS_ARGS2(BASE)          ((uint32_t)(BASE)  + (uint32_t)0X0Bu)
#define COMMANDS_ARGS3(BASE)          ((uint32_t)(BASE)  + (uint32_t)0X0Du)

#define SCORE_MAGIC(BASE)             ((uint32_t)(BASE)  + (uint32_t)0X00)
#define SCORE_FLAGS(BASE)             ((uint32_t)(BASE)  + (uint32_t)0X04)
#define SCORE_PNAME(BASE)             ((uint32_t)(BASE)  + (uint32_t)0X05)
#define SCORE_GNAME(BASE)             ((uint32_t)(BASE)  + (uint32_t)0X11)
#define SCORE_HRECORD(BASE)           ((uint32_t)(BASE)  + (uint32_t)0X1D)
#define SCORE_CRECORD(BASE)           ((uint32_t)(BASE)  + (uint32_t)0X1F)


extern SPI             spi;
extern struct SRAM_MAP sram_map;

void sharedMemoryInit();
void pushNode(struct NODE *node);
void popNode();
void loadBufferFromStack();

void bufferWrite(uint8_t g, uint8_t r, uint8_t b,  uint32_t index);
void bufferClear();

void updateNode(struct NODE *node, uint32_t base);
void readNode(struct NODE *node, uint32_t base);

void loadFood(struct NODE *food);
void loadCommand();
void getCommand();
void loadScore();
void getScore();

#elif (GAME == SPACE)
#define FRAME 5

#elif (GAME == PONG)

#endif
#endif