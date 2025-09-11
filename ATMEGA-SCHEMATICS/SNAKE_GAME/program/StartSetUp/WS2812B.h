#ifndef WS2812B_H
#define WS2812B_H
#include "includes.h"
#include "start.h"
#include "DataStructures.h"


void loadGame(SnakeBelly *belly);
void engGame(SnakeBelly *belly);
void startGame(SnakeBelly *belly);
void walk(SnakeBelly *belly);

void playColor();
void cellbycell();
void clear();
void snake(int size);

#endif