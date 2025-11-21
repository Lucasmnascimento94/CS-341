#ifndef SCREEN_H
#define SCREEN_H
#ifdef __cplusplus
extern "C" {
#endif
#include <avr/io.h>
#include "string.h"
#include "stdint.h"
#include "stdbool.h"
#include "i2c.h"
#include <stdio.h>
#include "string.h"

// default values.
#define RS_BIT      0
#define RW_BIT      1
#define E_BIT       2
#define BT_BIT      3

#define DL_BIT  4
#define N_BIT   3
#define F_BIT   2
#define S_C_BIT 3
#define R_L_BIT 2
#define D_BIT   2
#define C_BIT   1
#define B_BIT   0
#define I_D_BIT 1
#define SH_BIT  0
#define R_W_BIT 1
#define RS_BIT  0
#define RAM_BIT 7

#define CLEAR_DISPLAY       0X01
#define HOME                0X02
#define ENTRY_MODE(I_D, SH)          ((0X04) | (I_D << I_D_BIT) | (SH<<SH_BIT))
#define DISPLAY_ON_OFF(D, C, B)      ((0X08) | (D<<D_BIT) | (C<<C_BIT) | (B<<B_BIT))
#define CURSOR(S_C, R_L)              ((0X10) | (S_C<<S_C_BIT) | (R_L<<R_L_BIT))
#define FUNCTION_SET(DL, N, F)        ((0X20) | (DL<<DL_BIT) | (N<<N_BIT) | (F<<F_BIT))



#define READ_BUSY_FLAG  (uint16_t)0x0200
#define WRITE_TO_RAM    (uint16_t)0x0400
#define READ_FROM_RAM   (uint16_t)0x0800

#define ENTRY_MODE_  

typedef struct {
    uint8_t I_D, SH;   // Entry mode
    uint8_t D, C, B;   // Display on/off, cursor, blink
    uint8_t S_C, R_L;  // Shift command fields (rarely used at init)
    uint8_t DL, N, F;  // Function set
}SCREEN_CONF;

typedef struct{
    uint8_t pcf8574_addr;
    uint8_t current_row;
    uint8_t current_column;
    SCREEN_CONF conf;
}SCREEN;

typedef struct {
    uint8_t pattern[8];
    uint8_t row;
    uint8_t col;
    uint8_t slot;
} LCDTile;

void screenInit(SCREEN *screen, bool default_conf);
void setCursorDDRAM(uint8_t pos, uint8_t pcf_address);
void screenWriteDDRAM(SCREEN *screen, char *buffer);
void setCursorCGRAM(uint8_t pos, uint8_t pcf_address);
void screenWriteCGRAM(SCREEN *screen, const uint8_t *pattern, uint8_t char_index);
uint8_t getAddress(uint8_t row, uint8_t column);
void screenClear(SCREEN *screen);
void screenSetCursor(SCREEN *screen, uint8_t row, uint8_t column);
void screenWriteAt(SCREEN *screen, uint8_t row, uint8_t column, char *text);
void screenPrintCustomChar(SCREEN *screen, uint8_t row, uint8_t col, uint8_t char_index);
void loadTileBatch(SCREEN *screen, LCDTile *tiles, uint8_t count);
void snakeGameIcon(SCREEN *screen);
void pacManIcon(SCREEN *screen);
void pongIcon(SCREEN *screen);
void spacecraftIcon(SCREEN *screen);
void updateGameIcon(SCREEN *screen, const char *game_name);
void updateGameScreen(SCREEN *screen, char *game_name, uint16_t current_score, uint16_t top_score);

#ifdef __cplusplus
}
#endif
#endif
