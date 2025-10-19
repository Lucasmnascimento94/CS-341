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


#define MODE_MASTER_POL 0X00
#define MODE_MASTER_INT 0X01
#define MODE_SLAVE_POL  0X10
#define MODE_SLAVE_INT  0X11
/*==============================================================================
 *  I2C Flags Opcodes
 *==============================================================================
 *============================================================================*/



 /*
SLA+W will be transmitted;ACK or NOT ACK will be received
 */
#define START_TRANSMITTED               0X08


/*
SLA+W will be transmitted; ACK or NOT ACK will be received
SLA+R will be transmitted; Logic will switch to Master Receiver mode
*/
#define START_RETRANSMITTED             0X10


/*
Data byte will be transmitted and ACK or NOT ACK will be received
Repeated START will be transmitted STOP condition will be transmitted and
TWSTO Flag will be reset STOP condition followed by a START condition will be
transmitted and TWSTO Flag will be reset
*/
#define SLA_PLUS_W_ACK          0X18


/*
Data byte will be transmitted and ACK or NOT ACK will
be received Repeated START will be transmitted
STOP condition will be transmitted and TWSTO Flag will be reset
STOP condition followed by a START condition will be
transmitted and TWSTO Flag will be reset
*/
#define SLA_PLUS_W_NOT_ACK       0X20


/*
Data byte will be transmitted and ACK or NOT ACK will
be received Repeated START will be transmitted
STOP condition will be transmitted and TWSTO Flag will be reset
STOP condition followed by a START condition will be
transmitted and TWSTO Flag will be reset
*/
#define DATA_BYTE_TRANSMITTED_ACK       0X28


/*
Data byte will be transmitted and ACK or NOT ACK will
be received Repeated START will be transmitted
STOP condition will be transmitted and
TWSTO Flag will be reset STOP condition followed by a START condition will be
transmitted and TWSTO Flag will be reset
*/
#define DATA_BYTE_TRANSMITTED_NO_ACK    0X30


/*
2-wire Serial Bus will be released and not addressed
Slave mode entered A START condition will be transmitted when the bus
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
    uint32_t TWBR_VAL; /*value for the clock generator*/
    uint32_t frequency; /*Maximum Speed frequency for the clock generator*/
    uint8_t prescaler; /**/
    uint8_t addr;
    uint8_t direction;
    uint8_t mode;
}I2C_TARGET;

typedef struct{
    char *data;
    size_t data_size;
    char *instruction;
    size_t intruction_size;
    uint8_t current_target_addr;
    uint8_t current_mode;
}I2C_PORT;

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


/*___________ProtocolControl____________*/
uint8_t start_POL(I2C_TARGET *target);
uint8_t start_INT(I2C_TARGET *target);
uint8_t i2cSTOP();

/*______________Writing______________*/
uint8_t i2cWrite_POL(I2C_PORT *port, I2C_TARGET *target);
uint8_t i2cWriteNoCtrl_POL(I2C_PORT *port);
uint8_t i2cWrite_INT(I2C_PORT *port, I2C_TARGET *target);
uint8_t i2cWriteNoCtrl_INT(I2C_PORT *port);

/*______________Reading______________*/
uint8_t i2cRead_POL(I2C_PORT *port, I2C_TARGET *target);
uint8_t i2cReadNoCtrl_POL(I2C_PORT *port, I2C_TARGET *target);
#endif