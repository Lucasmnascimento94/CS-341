#ifndef SHARED_H
#define SHARED_H

#include "includes.h"
#include "spi.h"
#include "sram.h"
#include "data_types.h"
/*_______________Header Names______________*/
#define MAGIC_BELLY         "SB"
#define MAGIC_CELL          "CL"
#define MAGIC_MAIL          "ML"
#define MAGIC_SCORE         "SC"

/*________Constant PreDefined Values______*/
#define PAGE_SIZE            32
#define NULL_PTR             (uint32_t)0x00
#define FLAG_DIRTY           0x00u
#define FLAG_VALID           0xA5u
#define SRAM_SIZE            0x1FFFF
#define BUFFER_SIZE         (65*65)

/*__________Base addresses Definition___________*/
#define GRID_BUFFER_BASE   ((uint32_t)0x10u)
#define BELLY_BASE         ((uint32_t)((uint32_t)GRID_BUFFER_BASE + (uint32_t)BUFFER_SIZE))
#define MAIL_BASE          ((uint32_t)((uint32_t)BELLY_BASE + 0x20u))
#define FOOD_START_BASE    ((uint32_t)((uint32_t)MAIL_BASE + 0x40u))
#define FOOD_MAX           ((uint32_t)4u)   /* allow space for 4 foods */
#define SCORE_BASE         ((uint32_t)((uint32_t)FOOD_START_BASE + 0x40u + 0xA0u))  /* == FOOD_START_BASE + 0xE0 */
#define CELL_START_BASE    ((uint32_t)((uint32_t)SCORE_BASE + 0xC0u))


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
struct MAIL{
    uint32_t cmdID;
    uint16_t arg1;
    uint16_t arg2;
    uint16_t arg3;
    uint16_t block_size;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct BELLY{
    uint8_t flags;
    uint16_t block_size;
    bool init;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CELLS{
    uint32_t current_base;
    uint32_t head_base;
    uint32_t tail_base;
    uint32_t food_base;
    uint16_t block_size;
    uint16_t count;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SRAM_MAP {
    struct SCORE score;
    struct BELLY belly ;
    struct CELLS cell;
    struct MAIL mail;
};
#pragma pack(pop)

#define SNAKE_BELLY_MAGIC(BASE)       ((uint32_t)(BASE) + (uint32_t)0x00u)
#define SNAKE_BELLY_FLAGS(BASE)       ((uint32_t)(BASE) + (uint32_t)0x03u)
#define SNAKE_BELLY_HEAD(BASE)        ((uint32_t)(BASE) + (uint32_t)0x05u)
#define SNAKE_BELLY_TAIL(BASE)        ((uint32_t)(BASE) + (uint32_t)0x0Au)
#define SNAKE_BELLY_FOOD(BASE)        ((uint32_t)(BASE) + (uint32_t)0x0Fu)
#define SNAKE_BELLY_COUNT(BASE)       ((uint32_t)(BASE) + (uint32_t)0x14u)
#define SNAKE_BELLY_DIRECTION(BASE)   ((uint32_t)(BASE) + (uint32_t)0x17u)
#define SNAKE_BELLY_WALK(BASE)        ((uint32_t)(BASE) + (uint32_t)0x19u)
#define SNAKE_BELLY_COLOR(BASE)       ((uint32_t)(BASE) + (uint32_t)0x1Bu)
#define SNAKE_BELLY_COLOR_FOOD(BASE)  ((uint32_t)(BASE) + (uint32_t)0x20u)
#define SNAKE_BELLY_PT(BASE)          ((uint32_t)(BASE) + (uint32_t)0x25u)
#define SNAKE_BELLY_END(BASE)         ((uint32_t)(BASE) + (uint32_t)0x27u)
#define SNAKE_BELLY_BEGIN(BASE)       ((uint32_t)(BASE) + (uint32_t)0x29u)

#define CELL_MAGIC(BASE)              ((uint32_t)(BASE) + (uint32_t)0x00u)
#define CELL_FLAGS(BASE)              ((uint32_t)(BASE) + (uint32_t)0x02u)
#define CELL_NEXT(BASE)               ((uint32_t)(BASE) + (uint32_t)0x03u)
#define CELL_PREV(BASE)               ((uint32_t)(BASE) + (uint32_t)0x07u)
#define CELL_VAL(BASE)                ((uint32_t)(BASE) + (uint32_t)0x0Bu)
#define CELL_I(BASE)                  ((uint32_t)(BASE) + (uint32_t)0x0Du)
#define CELL_J(BASE)                  ((uint32_t)(BASE) + (uint32_t)0x0Eu)
#define CELL_POISON(BASE)             ((uint32_t)(BASE) + (uint32_t)0x0Fu)


#define MAIL_MAGIC(BASE)              ((BASE)  + 0X00)
#define MAIL_FLAGS(BASE)              ((BASE)  + 0X03)
#define MAIL_CMDID(BASE)              ((BASE)  + 0X05)
#define MAIL_ARGS1(BASE)              ((BASE)  + 0X0A)
#define MAIL_ARGS2(BASE)              ((BASE)  + 0X0D)
#define MAIL_ARGS3(BASE)              ((BASE)  + 0X10)

#define SCORE_MAGIC(BASE)             ((BASE)  + 0X00)
#define SCORE_FLAGS(BASE)             ((BASE)  + 0X03)
#define SCORE_PNAME(BASE)             ((BASE)  + 0X05)
#define SCORE_GNAME(BASE)             ((BASE)  + 0X12)
#define SCORE_HRECORD(BASE)           ((BASE)  + 0X1F)
#define SCORE_CRECORD(BASE)           ((BASE)  + 0X22)


extern SPI          spi;
extern struct SRAM_MAP sram_map;

void loadInitials();
void pushCell(struct Cell *cell);
void popCell();

void updateCell(struct Cell *cell, uint32_t base);
void readCell(struct Cell *cell, uint32_t base);

void loadBelly(SnakeBelly *belly);
void loadMail();
void loadScore();


void testMem();

#endif