#ifndef PONG_H
#define PONG_H

#include "includes.h"
#include "shared_memory.h"
#include "WS2812B.h"
#include "colors.h"
#include "start.h"
#include "stdio.h"
#include "string.h"


/*________COMMAND_OPCODES_______*/
#define PONG_STALL   0x00
#define PONG_LEFT    0x01
#define PONG_RIGHT   0x02
#define PONG_UP      0x03
#define PONG_DOWN    0x04
#define END_GAME     0x05

/*________Game Constants_______*/
#define PADDLE_LENGTH 4
#define PADDLE_WIDTH 1
#define PADDLE_LEFT_X 2
#define PADDLE_RIGHT_X 45
#define BALL_SIZE 1
#define SCORE_LIMIT 5

#define BALL_COLOR COLOR_WHITE
#define LEFT_PADDLE_COLOR COLOR_GREEN
#define RIGHT_PADDLE_COLOR COLOR_RED



#endif