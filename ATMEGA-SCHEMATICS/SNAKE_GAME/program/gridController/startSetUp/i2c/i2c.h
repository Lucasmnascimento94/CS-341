#ifndef I2C_H
#define I2C_H
#include "includes.h"

#define I2C_PRESCALER 4
#define I2C_SCL(twbr) (uint32_t)(F_CPU / (16 + 2*(twbr)*I2C_PRESCALER))
#define slave_count 1

#define PCF8574_ADDR_READ  (0x4F >> 1)
#define PCF8574_ADDR_WRITE (0X4E >> 1)
#define PCF8574_INDEX       0
#define I2C_WRITE 0
#define I2C_READ  1
#define I2C_TWSR_FLAG_MASK 0XF8

/*==============================================================================
 *  I2C Flags Opcodes
 *==============================================================================
 *============================================================================*/



 /*
SLA+W will be transmitted;
ACK or NOT ACK will be received
 */
#define START_TRANSMITTED               0X08


/*
SLA+W will be transmitted;
ACK or NOT ACK will be received
SLA+R will be transmitted;
Logic will switch to Master Receiver mode
*/
#define START_RETRANSMITTED             0X10


/*
Data byte will be transmitted and ACK or NOT ACK will
be received
Repeated START will be transmitted
STOP condition will be transmitted and
TWSTO Flag will be reset
STOP condition followed by a START condition will be
transmitted and TWSTO Flag will be reset
*/
#define SLA_PLUS_W_ACK          0X18


/*
Data byte will be transmitted and ACK or NOT ACK will
be received
Repeated START will be transmitted
STOP condition will be transmitted and
TWSTO Flag will be reset
STOP condition followed by a START condition will be
transmitted and TWSTO Flag will be reset
*/
#define SLA_PLUS_W_NOT_ACK       0X20


/*
Data byte will be transmitted and ACK or NOT ACK will
be received
Repeated START will be transmitted
STOP condition will be transmitted and
TWSTO Flag will be reset
STOP condition followed by a START condition will be
transmitted and TWSTO Flag will be reset
*/
#define DATA_BYTE_TRANSMITTED_ACK       0X28


/*
Data byte will be transmitted and ACK or NOT ACK will
be received
Repeated START will be transmitted
STOP condition will be transmitted and
TWSTO Flag will be reset
STOP condition followed by a START condition will be
transmitted and TWSTO Flag will be reset
*/
#define DATA_BYTE_TRANSMITTED_NO_ACK    0X30


/*
2-wire Serial Bus will be released and not addressed
Slave mode entered
A START condition will be transmitted when the bus
becomes freeWSR & 0xF8
*/
#define ARBITRATION_LOST                0X38


/*==============================================================================
 *  Registers bitMap
 *==============================================================================
 *  Reference:
 *    - I2C Application Note : ./documentation/protocols/SPI.md
 *
 *    CONTROL_REGISTER [7-0]
 *    [TWINT] [TWEA] [TWSTA] [TWSTO] [TWWC] [TWEN] [-] [TWIE]
 *    
 *    usage patterns) are documented in the application notes listed above.
 *
 *============================================================================*/
typedef struct{
    uint8_t addr;
    uint8_t count;
}I2C_MASTER;

typedef struct{
    uint8_t addr;
    uint8_t count;
}I2C_SLAVE;

typedef struct{
    char *data;
    char *instruction;
    uint16_t twbr;
    uint8_t prescaler;
    bool read_mode;
    bool master_mode;
    bool interrupt_mode;
    bool polling_mode;
    I2C_MASTER **master;
    I2C_SLAVE **slave;
    uint32_t frequency;
    uint32_t TWBR_VAL;
}I2C_CONF;

// TWDR -> data register

/*==============================================================================
 *  I2C Master Mode
 *==============================================================================
 *  Reference:
 *    - SPI Application Note : ./documentation/protocols/SPI.md
 *    - SRAM Application Note: ./documentation/grid/SRAM.md
 *
 *  Description:
 *    This section contains configuration and setup for the SPI peripheral
 *    on the ATmega168A. All implementation details (registers, data flow,
 *    usage patterns) are documented in the application notes listed above.
 *
 *============================================================================*/


 void i2cConfig(I2C_CONF *conf);
 uint8_t i2cMasterStart_POL(I2C_CONF *conf);
 void i2cMasterWrite_POL(I2C_CONF *conf, uint8_t slave_index);
 void i2cMasterRead_POL(I2C_CONF *conf, char *data, int size);

 void i2cMasterWrite_POL_START(I2C_CONF *conf, uint8_t slave_index);
 void i2cMasterWrite_POL_SEND(I2C_CONF *conf, uint8_t slave_index);
 void i2cMasterWrite_POL_STOP(I2C_CONF *conf, uint8_t slave_index);
#endif