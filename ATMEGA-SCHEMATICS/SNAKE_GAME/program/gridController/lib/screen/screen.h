#ifndef SCREEN_H
#define SCREEN_H
#include "includes.h"
#include "i2c.h"

#define RS      0
#define RW      1
#define E       2
#define BT      3


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
#define ENTRY_MODE          ((0X04) | (I_D << 1) | (SH))
#define DISPLAY_ON_OFF      ((0X08) | (2<<D) | (1<<C) | (B))
#define CURSOR              ((0X10) | (3<<S_C) | (2<<R_L))
#define FUNCTION_SET        ((0X20) | (4<<DL) | (3<<N) | (2<<F))


#define READ_BUSY_FLAG  0xFF
#define WRITE_TO_RAM    0xFF
#define READ_FROM_RAM   0xFF

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
}SCREEN_CONF;

void screenInit(I2C_CONF *i2c_conf, I2C_SLAVE *i2c_slave, SCREEN_CONF *screen_conf);
void screenWrite(I2C_CONF *i2c_conf, SCREEN_CONF *screen_conf, uint8_t line);
#endif