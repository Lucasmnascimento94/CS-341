/*==============================================================================
 *  I2C DRIVER — POLLING + INTERRUPT MODES (AVR TWI)
 *  File: <replace with filename>
 *  Brief: <one-liner about what this file covers>
 *  Author: <you>
 *  Notes: Target: ATmega168/328-class, uses TWBR/TWSR/TWCR/TWDR registers.
 *==============================================================================*/

#include "i2c.h"
#include "string.h"
#include "uart.h"
#include "stdio.h"

/*==============================================================================
 *  SECTION: CLOCK & GPIO INITIALIZATION
 *------------------------------------------------------------------------------
 *  Purpose: Configure prescaler, TWBR bit rate, and set SDA/SCL pins as needed.
 *  Includes: i2cClockConfig(), i2cModeConf(), i2cInit(), and conf helpers.
 *==============================================================================*/
    

void i2cConfMasterPol(){
    /*to do*/
}

void i2cConfMasterInt(){
    /*to do*/
}

void i2cConfSlavePol(){
    /*to do*/
}

void i2cConfSlaveInt(){
    /*to do*/
}

void i2cInit(I2C_CONF *conf){
    DDRC |= (1<<PC5) | (1<<PC4);
    PORTC |= (1<<PC5) | (1<<PC4);
    i2cClockConfig(conf);
    i2cModeConf(conf);
}

void i2cClockConfig(I2C_CONF *conf){
/*Note:
Slave operation does not depend on Bit Rate or Prescaler settings, but the CPU clock 
frequency in the Slave must be at least 16 times higher than the SCL frequency. 
*/

    switch (conf->prescaler){
        case 1:
            TWSR &= ~(1<<TWPS1) & ~(1<<TWPS0);
            break;
        case 4:
            TWSR &= ~(1<<TWPS1);
            TWSR |=  (1<<TWPS0);
            break;
        case 16:
            TWSR |=  (1<<TWPS1);
            TWSR &= ~(1<<TWPS0);
            break;
        case 64:
            TWSR |=  (1<<TWPS1);
            TWSR |=  (1<<TWPS0);
            break;
        default:
            /*to do*/
            break;

    }

    uint32_t twbr_num = (conf->f_cpu / conf->frequency);
    uint32_t twbr_den = 2*(conf->prescaler);
    TWBR = (uint8_t) ((twbr_num - 16)/twbr_den);
}

void i2cModeConf(I2C_CONF *conf){
    switch (conf->mode){
        case MODE_MASTER_POL:
            i2cConfMaster_POL();
            break;
        case MODE_MASTER_INT:
            i2cConfMaster_INT();
            break;
        case MODE_SLAVE_POL:
            i2cConfSlave_POL();
            break;
        case MODE_SLAVE_INT:
            i2cConfSlave_INT();
            break;
        default:
            uartWrite_("Err..i2cConf<Invalid Mode>");
            return;
    }
}


/*==============================================================================
 *  SECTION: HELPERS / BUS CONTROL
 *------------------------------------------------------------------------------
 *  Purpose: Small helpers for repeated bus patterns (START/STOP/RETRY/STATUS).
 *  Includes: i2cPolHelper(), twsrFlagHandler(), and status decoding.
 *==============================================================================*/
uint8_t i2cStartPOL(uint8_t address, uint8_t mode){
    /* Modify address byte on I2C protocol (SLA+W)
       Writing::direction==0 | Reading::direction==1*/
    TWCR |= (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);        // Send Start Signal on SDA bus
    while(!(TWCR & (1<<TWINT)));                        // Check for hardware feedback

    TWDR=address;                                       // Write SLA+W onto data register
    TWCR &= ~(1<<TWSTA);                                // Clear STA bit
    TWCR |= (1<<TWINT) | (1<<TWEN);                     // Clear flag
    while(!(TWCR & (1<<TWINT)));                        // Wait for hardware flag

    return TWSR & I2C_TWSR_FLAG_MASK;                   // Return status flag
}

uint8_t i2cSTOP(){
    TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN); // Send a Stop condition*/
    return TWSR & I2C_TWSR_FLAG_MASK; 
}

uint8_t i2cPolHelper(uint8_t address, uint8_t mode){
    uint8_t tries = 0;                                                      // Counter
    uint8_t status = 0;                                                     // status flag

    for(;status != SLA_PLUS_W_ACK; tries++){                                // Check for flag in polling mode
        if(tries>=10) return status;                                        // Return if it failed 10 times.
        status = (mode == I2C_READ)? \
        i2cStartPOL(address, I2C_READ):i2cStartPOL(address, I2C_WRITE);     
    
        if(status == SLA_PLUS_W_ACK) break;                                 // break loop if ACK is received
        TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN);                        // Reset and send STOP signal, to start over.
        _delay_us(1);
    }
    return status;
 }

void twsrFlagHandler(char *msg){
    char str[60] = {0};
    strcat(str, msg);
    switch (TWSR & I2C_TWSR_FLAG_MASK){
        case START_TRANSMITTED:
            strcat(str, "... Flag <START_RETRANSMITTED>\n");
            uartWrite_(str);
            break;
        case SLA_PLUS_W_ACK:
            strcat(str, "... Flag <SLA_PLUS_W_ACK>\n");
            uartWrite_(str);
            break;
        case SLA_PLUS_W_NOT_ACK:
            strcat(str, "... ErrorFlag <SLA_PLUS_W_NOT_ACK>\n");
            uartWrite_(str);
            break;
        case DATA_BYTE_TRANSMITTED_ACK:
            strcat(str, "... Flag <DATA_BYTE_TRANSMITTED_ACK>\n");
            uartWrite_(str);
            break;
        case DATA_BYTE_TRANSMITTED_NO_ACK:
            strcat(str, "... ErrorFlag <DATA_BYTE_TRANSMITTED_NO_ACK>\n");
            uartWrite_(str);
            break;
        case ARBITRATION_LOST:
            strcat(str, "... ErrorFlag <ARBITRATION_LOST>\n");
            uartWrite_(str);
            break;
        default:
            uartWrite_("Unknown FLag\n");
    }
}


/*==============================================================================
 *  SECTION: WRITE / TX  (POLLING)
 *------------------------------------------------------------------------------
 *  Purpose: Routines that *send* bytes/buffers on the I²C bus in polling mode.
 *  Includes: i2cStartPOL(), i2cWritePOL(), i2cWritePOL_(), i2cSTOP().
 *==============================================================================*/
uint8_t i2cWritePOL(char *buffer, size_t size, uint8_t address){
    /*Sanity Check*/
    if(buffer == NULL) return 0x01;
    
    /*Mode Adjustment*/
    if(i2cPolHelper(address) != SLA_PLUS_W_ACK){
        twsrFlagHandler("i2cWrite_Pol");
    }
    else{
        for(size_t i=0; i<size; i++){
        while(!(TWCR & (1<<TWINT)));                      // Wait for Hardware flag
        TWDR = buffer[i];                             // Write data to I2C register
        TWCR = (1<<TWINT) | (1<<TWEN);                    // Send data
        while(TWSR != DATA_BYTE_TRANSMITTED_ACK){} // Wait for hardware flag
        }
    }
    
    TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN);          // Send a Stop condition 
    return TWSR & I2C_TWSR_FLAG_MASK;                             
 }

 uint8_t i2cWritePOL_(char *buffer, size_t size){
    if(buffer == NULL) return 0x01;
    for(size_t i=0; i<size; i++){
        while(!(TWCR & (1<<TWINT)));
        TWDR = buffer[i];
        TWCR = (1<<TWINT) | (1<<TWEN); 
        while(TWSR != DATA_BYTE_TRANSMITTED_ACK){}
    }
    return TWSR != DATA_BYTE_TRANSMITTED_ACK;
 }

 uint8_t i2cReadPol(buffer *data, size_t size, uint8_t address){
    /*Sanity Check*/
    if(buffer == NULL) return 0x01;
    
    /*Mode Adjustment*/
    if(i2cPolHelper(address) != SLA_PLUS_W_ACK){
        twsrFlagHandler("i2cRead_Pol");
    }
    else{
        for(size_t i=0; i<size; i++){
        while(!(TWCR & (1<<TWINT)));                      // Wait for Hardware flag
        TWDR = buffer[i];                             // Write data to I2C register
        TWCR = (1<<TWINT) | (1<<TWEN);                    // Send data
        while(TWSR != DATA_BYTE_TRANSMITTED_ACK){} // Wait for hardware flag
        }
    }
    
    TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN);          // Send a Stop condition 
    return TWSR & I2C_TWSR_FLAG_MASK;                                
}

uint8_t i2cReadPOL_(char *buffer, size_t size){
    if(buffer == NULL) return 0x01;
    for(size_t i=0; i<size; i++){
        while(!(TWCR & (1<<TWINT)));
        TWDR = buffer[i];
        TWCR = (1<<TWINT) | (1<<TWEN); 
        while(TWSR != DATA_BYTE_TRANSMITTED_ACK){}
    }
    return TWSR != DATA_BYTE_TRANSMITTED_ACK;
 }


 