#ifndef SHARED_H
#define SHARED_H
#include "stdint.h"
#include "stddef.h"
#include "snake.h"
#include "stdbool.h"
#include "sram.h"

/*_______________Header Names______________*/
#define MAGIC_BELLY         "SB"
#define MAGIC_CELL          "CL"
#define MAGIC_MAIL          "ML"
#define MAGIC_SCORE         "SC"

/*________Constant PreDefined Values______*/
#define PAGE_SIZE           32
#define NULL_PTR            0xFFFFFFFFu
#define FLAG_DIRTY          0x00u
#define FLAG_VALID          0xA5u
#define SRAM_SIZE           0x1FFFF
#define BUFFER_SIZE         (65*65)

/*__________Base addresses Definition___________*/
#define GRID_BUFFER_BASE    0X00
#define BELLY_BASE          (0X00 + (GRID_BUFFER_BASE + BUFFER_SIZE))
#define MAIL_BASE           (0X20 + (BELLY_BASE))
#define FOOD_START_BASE     (0x40 + (MAIL_BASE))
#define FOOD_MAX            4   // Allow space for 4 foods 
#define SCORE_BASE          (0XA0 + (FOOD_START_BASE + 0X40))
#define CELL_START_BASE     (0XC0 + (SCORE_BASE))      


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
struct PACK{
    uint32_t current_base;
    uint32_t head_base;
    uint32_t tail_base;
    uint16_t block_size;
    uint16_t count;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SRAM_MAP {
    struct SCORE score;
    struct BELLY belly ;
    struct PACK cell;
    struct PACK buffer;
    struct MAIL mail;
};
#pragma pack(pop)

#define SNAKE_BELLY_MAGIC(BASE)       ((BASE)  + 0X00)
#define SNAKE_BELLY_FLAGS(BASE)       ((BASE)  + 0X03)
#define SNAKE_BELLY_HEAD(BASE)        ((BASE)  + 0X05)
#define SNAKE_BELLY_TAIL(BASE)        ((BASE)  + 0X0A)
#define SNAKE_BELLY_FOOD(BASE)        ((BASE)  + 0X0F)
#define SNAKE_BELLY_COUNT(BASE)       ((BASE)  + 0X12)
#define SNAKE_BELLY_DIRECTION(BASE)   ((BASE)  + 0X15)
#define SNAKE_BELLY_WALK(BASE)        ((BASE)  + 0X17)
#define SNAKE_BELLY_COLOR(BASE)       ((BASE)  + 0X19)
#define SNAKE_BELLY_COLOR_FOOD(BASE)  ((BASE)  + 0X1E)
#define SNAKE_BELLY_PT(BASE)          ((BASE)  + 0X23)
#define SNAKE_BELLY_END(BASE)         ((BASE)  + 0X25)
#define SNAKE_BELLY_BEGIN(BASE)       ((BASE)  + 0X27)


#define CELL_MAGIC(BASE)              ((BASE)  + 0X00)
#define CELL_FLAGS(BASE)              ((BASE)  + 0X03)
#define CELL_NEXT(BASE)               ((BASE)  + 0X05)
#define CELL_PREV(BASE)               ((BASE)  + 0X0A)
#define CELL_VAL(BASE)                ((BASE)  + 0X0F)
#define CELL_I(BASE)                  ((BASE)  + 0X12)
#define CELL_J(BASE)                  ((BASE)  + 0X14)
#define CELL_POISON(BASE)             ((BASE)  + 0X16)


#define MAIL_MAGIC(BASE)              ((BASE)  + 0X00)
#define MAIL_FLAGS(BASE)              ((BASE)  + 0X03)
#define MAIL_CMDID(BASE)              ((BASE)  + 0X05)
#define MAIL_ARGS1(BASE)              ((BASE)  + 0X0A)
#define MAIL_ARGS2(BASE)              ((BASE)  + 0X0D)
#define MAIL_ARGS3(BASE)              ((BASE)  + 0X10)

#define SCORE_MAGIC(BASE)             ((BASE)  + 0X00)
#define SCORE_FLAGS(BASE)             ((BASE)  + 0X03)
#define SCORE_PNAME(BASE)             ((BASE)  + 0X05)
#define SCORE_GNAME(BASE)             ((BASE)  + 0X0A)
#define SCORE_HRECORD(BASE)           ((BASE)  + 0X0D)
#define SCORE_CRECORD(BASE)           ((BASE)  + 0X10)


extern SPI          spi;
extern struct SRAM_MAP sram_map;

void sramLoadInitials();
void sramPushCell(struct Cell *cell);
void sramPopCell();

void sramLoadBelly(SnakeBelly *belly);
void sramLoadMail();
void sramLoadScore();


#endif