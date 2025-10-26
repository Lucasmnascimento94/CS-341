#ifndef TYPES_H
#define TYPES_H

#include "includes.h"

typedef struct Cell {
    uint32_t next_addr; // -> 2bytes
    uint32_t prev_addr; // -> 2bytes
    uint16_t val; // -> 2bytes
    uint8_t i; // -> 1 byte
    uint8_t j; // -> 1 byte
    bool poison; // -> 1 byte -> TOTAL : 9bytes
}Cell;

typedef struct {
    struct Cell head;
    struct Cell tail;
    struct Cell food;
    uint16_t count;
    uint8_t direction;
    bool walk;
    uint32_t color;
    uint32_t color_food;
    uint8_t pt;
    bool end;
    bool begin;
}SnakeBelly;

#endif