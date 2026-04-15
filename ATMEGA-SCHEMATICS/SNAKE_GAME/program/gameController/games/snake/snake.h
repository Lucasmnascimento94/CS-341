/**
 * @file    snake.h
 *
 * @author  Lucas Nascimento <lucas@hausintelligence.com>
 * @version 1.0.0
 * @date    2025-09-11
 *
 * @target  ATmega168 @ 16 MHz (F_CPU=16000000UL)
 * @deps    avr-libc
 *
 * @copyright
 * Copyright (c) 2025 Lucas Nascimento
 *
 * @changelog
 * - 1.2.0 (2025-09-11): Switched WS2812 timing to SBI/CBI, added PCINT joystick.
 * - 1.1.0 (2025-09-05): Added cycle-accurate delays.
 * - 1.0.0 (2025-08-30): Initial release.
 */


#ifndef SNAKE_H
#define SNAKE_H

#include "includes.h"
#include "WS2812B.h"
#include "grid.h"
#include "shared_memory.h"
#include "DataStructures.h"
#include "colors.h"
#include "data_types.h"

void push(SnakeBelly *belly, uint16_t val, uint8_t i, uint8_t j, bool poison);
void pop(SnakeBelly *belly);
void popAll(SnakeBelly *belly);
void insertion_sort(uint16_t arr[], uint16_t n);


/*============================================================================================*
 * RENDER — framebuffer/bitset and scanout                                                     *
 *============================================================================================*/
void walk(SnakeBelly *belly, struct Cell *food);
void initSnake(SnakeBelly *belly, struct Cell *food);

/*============================================================================================*
 * RULES — collisions, growth, scoring, bounds, difficulty                                    *
 *============================================================================================*/
bool ruleCheck(uint16_t food_addr);
bool foodCheck(uint16_t address, bool *eatFood);
/*============================================================================================*
 * EFFECTS — sound, LED flashes, animations                                                    *
 *============================================================================================*/
void move(SnakeBelly *belly, uint8_t new_i, uint8_t new_j, uint8_t k, uint8_t I);
void move_grow(SnakeBelly *belly, uint8_t new_i, uint8_t new_j, uint8_t k, uint8_t I);

void gameInit();
void gameEnd();
void gameLose(SnakeBelly *belly, struct Cell *food);
void gameWin(SnakeBelly *belly, struct Cell *food);
void foodPoison(SnakeBelly *belly);
void foodEat(SnakeBelly *belly);
void foodLoosing(Cell *food);


#endif