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

/*________NODE_OPCODES_______*/
#define SPECIAL_1 0X01
#define SPECIAL_2 0X02
#define FOOD_POISON 0X03

/*________COMMAND_OPCODES_______*/
#define WALK_UP 0X01
#define WALK_DOWN 0X02
#define WALK_LEFT 0X03
#define WALK_RIGHT 0X04
#define END_GAME   0X05

#define ARG_POISON_FOOD 0X06
#define ARG_COLLISION 0X07

#include "includes.h"
#include "string.h"
#include "colors.h"

#define SNAKE_COLOR COLOR_YELLOW
#define FOOD_COLOR COLOR_GREEN

/* ________________GAME RULES_________________
CAN'T DO:
1 - Hit own body
2 - Eat Poisoned Food
*/

/*============================================================================================*
 * RENDER — framebuffer/bitset and scanout                                                     *
 *============================================================================================*/
void walk();
void initSnake();
void generateFood();
/*============================================================================================*
 * RULES — collisions, growth, scoring, bounds, difficulty                                    *
 *============================================================================================*/
bool collisionCheck(uint32_t new_node_addr);
bool foodCheck(uint32_t new_node_addr, bool *poison);
/*============================================================================================*
 * EFFECTS — sound, LED flashes, animations                                                    *
 *============================================================================================*/


void initialAnimation();


#endif