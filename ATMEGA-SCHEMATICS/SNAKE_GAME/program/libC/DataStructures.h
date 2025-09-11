#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H
#include <includes.h>


typedef struct Cell {
    struct Cell *next;
    uint8_t val;
    uint32_t color;
    uint8_t poison;
}Cell;

typedef struct {
    struct Cell *head;
    struct Cell *tail;
    uint32_t color_food;
    uint16_t count;
    uint8_t direction;
    bool walk;
}SnakeBelly;

typedef struct {
    struct Cell *head;
    struct Cell *tail;
    uint16_t count;
}Queue;

void push(SnakeBelly *belly, bool poison);
bool pop(SnakeBelly *belly);

#endif

