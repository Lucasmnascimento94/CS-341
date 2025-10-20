#ifndef ALLOCATION_H
#define ALLOCATION_H
#include "includes.h"
#include "spi.h"
#include "uart.h"
#include "i2c.h"
#include "screen.h"
#include "WS2812B.h"
#include "snake.h"

extern SnakeBelly       belly;
extern struct Cell      food;
extern I2C_TARGET       i2c_target;
extern I2C_PORT         i2c_port;
extern SCREEN_CONF      screen;

uint8_t allocateVar(void);

#endif