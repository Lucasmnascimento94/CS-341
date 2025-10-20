#ifndef SCREEN_H
#define SCREEN_H
#include "includes.h"
#include "i2c.h"

#define RS      0
#define RW      1
#define E       2
#define BT      3

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

#define DDRAM   1
#define CGRAM   0
#define I_D     1
#define SH      0
#define D       1
#define C       1
#define B       1
#define S_C     0
#define R_L     1
#define DL      0   // 4-bits mode
#define N       1   // 2 lines mode
#define F       0   // small font


#define CLEAR_DISPLAY       0X01
#define HOME                0X02
#define ENTRY_MODE          ((0X04) | (I_D << I_D_BIT) | (SH<<SH_BIT))
#define DISPLAY_ON_OFF      ((0X08) | (D<<D_BIT) | (C<<C_BIT) | (B<<B_BIT))
#define CURSOR              ((0X10) | (S_C<<S_C_BIT) | (R_L<<R_L_BIT))
#define FUNCTION_SET        ((0X20) | (DL<<DL_BIT) | (N<<N_BIT) | (F<<F_BIT))


#define READ_BUSY_FLAG  (uint16_t)0x0200
#define WRITE_TO_RAM    (uint16_t)0x0400
#define READ_FROM_RAM   (uint16_t)0x0800

#define ENTRY_MODE_                       
typedef struct{
    uint8_t pcf8574_addr;
    bool mode_4bit;
    bool mode_4line;
    char *line1;
    char *line2;
    char *line3;
    char *line4;
    uint8_t position_line;
    uint8_t position_column;
    char *data;
    char *instruction;
}SCREEN_CONF;

void screenInit(I2C_PORT *port, I2C_TARGET *target);
void screenWrite(I2C_PORT *port, I2C_TARGET *target, SCREEN_CONF *screen);
void setCursorAt(I2C_PORT *port, I2C_TARGET *target, uint8_t row, uint8_t col);
void screenPrintAt(I2C_PORT *port, I2C_TARGET *target, uint8_t row, uint8_t col, const char *text);
void screenClearAt(I2C_PORT *port, I2C_TARGET *target, uint8_t row, uint8_t col, uint8_t length);
#endif