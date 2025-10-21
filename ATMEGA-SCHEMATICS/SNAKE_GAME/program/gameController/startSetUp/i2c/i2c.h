/* I2C/TWI — Public API  |  i2c.h
 * Author: Lucas Nascimento (lucas@hausintelligence.com))
 * Project: I2C Driver (AVR ATmega168/328)
 * Version: v1.1  |  Date: <10-20-2025>  |  License: Open Source
 * API Docs: API and reference note found in this repo
 * Repo: https://github.com/Lucasmnascimento94/CS-341/tree/SNAKE_V4_PROGRAMMER_LUCAS/ATMEGA-SCHEMATICS/SNAKE_GAME/program/gameController/startSetUp/i2c
 */


#ifndef I2C_H
#define I2C_H
#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include "stddef.h"
#include <stdbool.h>

/*──────────── Macros ────────────*/
#define I2C_WRITE 0
#define I2C_READ  1
#define I2C_TWSR_FLAG_MASK 0XF8

#define MODE_MASTER_POL 0X00
#define MODE_MASTER_INT 0X01
#define MODE_SLAVE_POL  0X10
#define MODE_SLAVE_INT  0X11

/*────────── TWSR status codes (master TX subset) ─────────*/
#define START_TRANSMITTED               0X08
#define START_RETRANSMITTED             0X10
#define SLA_PLUS_W_ACK                  0X18
#define SLA_PLUS_W_NOT_ACK              0X20
#define DATA_BYTE_TRANSMITTED_ACK       0X28
#define DATA_BYTE_TRANSMITTED_NO_ACK    0X30
#define ARBITRATION_LOST                0X38

/*────────── Config struct ─────────*/
typedef struct {
    uint32_t TWBR_VAL;          /* Optional precomputed TWBR; 0 to compute via f_cpu/frequency/prescaler */
    uint32_t frequency;         /* Target SCL frequency (Hz) */
    uint8_t prescaler;          /* 1, 4, 16, or 64 */
    uint8_t mode;               /* MODE_* */
    uint32_t f_cpu;             /* CPU clock (Hz) */
}I2C_CONF;

/*___________CLOCK & GPIO ____________*/
 void i2cInit(I2C_CONF *conf);
 void i2cClockConfig(I2C_CONF *conf);
 void i2cModeConf(I2C_CONF *conf);


/*______________HELPERS_______________*/
uint8_t i2cStartPol(uint8_t address, uint8_t mode);
uint8_t i2cStop();

/*_________Writing Polling___________*/
uint8_t i2cWritePol(char *buffer, size_t size, uint8_t address);
uint8_t i2cWritePol_(char *buffer, size_t size);

/*_________Reading Polling___________*/
uint8_t i2cReadPol(char *data, size_t size, uint8_t address);
uint8_t i2cReadPol_(char *buffer, size_t size);


#ifdef __cplusplus
}
#endif
#endif