#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H
#include <includes.h>


typedef struct Cell {
    struct Cell *next; // -> 2bytes
    struct Cell *prev; // -> 2bytes
    uint16_t val; // -> 2bytes
    uint8_t i; // -> 1 byte
    uint8_t j; // -> 1 byte
    bool poison; // -> 1 byte -> TOTAL : 9bytes
}Cell;

typedef struct {
    struct Cell *head;
    struct Cell *tail;
    struct Cell *food;
    uint16_t count;
    uint8_t direction;
    bool walk;
    uint32_t color;
    uint32_t color_food;
    uint8_t pt;
    bool end;
    bool begin;
}SnakeBelly;

typedef struct {
    struct Cell *head;
    struct Cell *tail;
    uint16_t count;
}Queue;

void push(SnakeBelly *belly, uint16_t val, uint8_t i, uint8_t j, bool poison);
void pop(SnakeBelly *belly);
void popAll(SnakeBelly *belly);
void insertion_sort(uint16_t arr[], uint16_t n);
#endif




/*






play >> 1405 - Display >> 1406 - Display >> 1407 - Display >> 1408 - Display >> 121 - while loop
Walk >> 1408 - Walk >> 1407 - Walk >> 1406 - Walk >> 1405 - 
Display >> 1404 - Display >> 1405 - Display >> 1406 - Display >> 1407 - Display >> 1408 - while loop
Walk >> 1407 - Walk >> 1406 - Walk >> 1405 - Walk >> 1404 - 
Display >> 1403 - Display >> 1404 - Display >> 1405 - Display >> 1406 - Display >> 1407 - while loop
Walk >> 1406 - Walk >> 1405 - Walk >> 1404 - Walk >> 1403 - 
Display >> 1402 - Display >> 1403 - Display >> 1404 - Display >> 1405 - Display >> 1406 - while loop
Walk >> 1405 - Walk >> 1404 - Walk >> 1403 - Walk >> 1402 - 
Display >> 1401 - Display >> 1402 - Display >> 1403 - Display >> 1404 - Display >> 1405 - while loop
Walk >> 1404 - Walk >> 1403 - Walk >> 1402 - Walk >> 1401 - 
Display >> 1152 - Display >> 1401 - Display >> 1402 - Display >> 1403 - Display >> 1404 - while loop
Walk >> 1403 - Walk >> 1402 - Walk >> 1401 - Walk >> 1152 - 
Display >> 1151 - Display >> 1152 - Display >> 1401 - Display >> 1402 - Displ











*/