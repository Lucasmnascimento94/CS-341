/*==============================================================================
 *  I2C DRIVER — POLLING + INTERRUPT MODES (AVR TWI)
 *  File: <replace with filename>
 *  Brief: <one-liner about what this file covers>
 *  Author: <you>
 *  Notes: Target: ATmega168/328-class, uses TWBR/TWSR/TWCR/TWDR registers.
 *==============================================================================*/
#include "i2c.h"

void i2cdefault(I2C_CONF *conf){
    conf->f_cpu = 20000000;
    conf->frequency = 400000;
    conf->mode = MODE_MASTER_POL;
    conf->prescaler = 1;
}

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

void i2cInit(I2C_CONF *conf, bool default_conf){
    DDRC  &= ~((1<<PC5) | (1<<PC4)); 
    PORTC |= (1<<PC5) | (1<<PC4);
    if(default_conf) i2cdefault(conf);
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

    uint8_t twbr = (uint8_t)((twbr_num - 16)/twbr_den);
    TWBR = (twbr < 2)?2U:twbr;
    conf->TWBR_VAL = TWBR;
}

void i2cModeConf(I2C_CONF *conf){
    switch (conf->mode){
        case MODE_MASTER_POL:
            i2cConfMasterPol();
            break;
        case MODE_MASTER_INT:
            i2cConfMasterInt();
            break;
        case MODE_SLAVE_POL:
            i2cConfSlavePol();
            break;
        case MODE_SLAVE_INT:
            i2cConfSlaveInt();
            break;
        default:
            return;
    }
}


/*==============================================================================
 *  SECTION: HELPERS / BUS CONTROL
 *------------------------------------------------------------------------------
 *  Purpose: Small helpers for repeated bus patterns (START/STOP/RETRY/STATUS).
 *  Includes: i2cPolHelper(), twsrFlagHandler(), and status decoding.
 *==============================================================================*/
uint8_t i2cStartPol(uint8_t address, uint8_t mode){
    /* Modify address byte on I2C protocol (SLA+W)
       Writing::direction==0 | Reading::direction==1*/
    uint8_t instruction = (address << 1) | mode;
    TWCR |= (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);        // Send Start Signal on SDA bus
    while(!(TWCR & (1<<TWINT)));                        // Check for hardware feedback
    TWDR=instruction;                                       // Write SLA+W onto data register
    TWCR &= ~(1<<TWSTA);                                // Clear STA bit
    TWCR |= (1<<TWINT) | (1<<TWEN);                     // Clear flag
    while(!(TWCR & (1<<TWINT)));                        // Wait for hardware flag

    return TWSR & I2C_TWSR_FLAG_MASK;                   // Return status flag
}

uint8_t i2cStop(){
    TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN); // Send a Stop condition*/
    return TWSR & I2C_TWSR_FLAG_MASK; 
}

uint8_t i2cPolHelper(uint8_t address, uint8_t mode){
    uint8_t tries = 0;                                                      // Counter
    uint8_t status = 0;                                                     // status flag

    for(;status != SLA_PLUS_W_ACK; tries++){                                // Check for flag in polling mode
        if(tries>=10) return status;                                        // Return if it failed 10 times.
        status = (mode == I2C_READ)? \
        i2cStartPol(address, I2C_READ):i2cStartPol(address, I2C_WRITE);     
    
        if(status == SLA_PLUS_W_ACK) break;                                 // break loop if ACK is received
        TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN);                        // Reset and send STOP signal, to start over.
        _delay_us(1);
    }
    return status;
 }

void twsrFlagHandler(){
    switch (TWSR & I2C_TWSR_FLAG_MASK){
        case START_TRANSMITTED:
            /*TO DO*/
            break;
        case SLA_PLUS_W_ACK:
            /*TO DO*/
            break;
        case SLA_PLUS_W_NOT_ACK:
            /*TO DO*/
            break;
        case DATA_BYTE_TRANSMITTED_ACK:
            /*TO DO*/
            break;
        case DATA_BYTE_TRANSMITTED_NO_ACK:
            /*TO DO*/
            break;
        case ARBITRATION_LOST:
            /*TO DO*/
            break;
        default:
            break;
            /*TO DO*/
    }
}


/*==============================================================================
 *  SECTION: WRITE / TX  (POLLING)
 *------------------------------------------------------------------------------
 *  Purpose: Routines that *send* bytes/buffers on the I²C bus in polling mode.
 *  Includes: i2cStartPOL(), i2cWritePOL(), i2cWritePOL_(), i2cSTOP().
 *==============================================================================*/
uint8_t i2cWritePol(char *buffer, size_t size, uint8_t address){
    /*Sanity Check*/
    if(buffer == NULL) return 0x01;
    
    /*Mode Adjustment*/
    if(i2cPolHelper(address, I2C_WRITE) != SLA_PLUS_W_ACK){
        twsrFlagHandler();
    }
    else{
        for(size_t i=0; i<size; i++){
        while(!(TWCR & (1<<TWINT)));                      // Wait for Hardware flag
        TWDR = buffer[i];                             // Write data to I2C register
        TWCR = (1<<TWINT) | (1<<TWEN);                    // Send data
        while((TWSR & I2C_TWSR_FLAG_MASK) != DATA_BYTE_TRANSMITTED_ACK){} // Wait for hardware flag
        }
    }
    
    return i2cStop();         // Send a Stop condition                            
 }

 uint8_t i2cWritePol_(char *buffer, size_t size){
    if(buffer == NULL) return 0x01;
    for(size_t i=0; i<size; i++){
        while(!(TWCR & (1<<TWINT)));
        TWDR = buffer[i];
        TWCR = (1<<TWINT) | (1<<TWEN); 
        while((TWSR & I2C_TWSR_FLAG_MASK) != DATA_BYTE_TRANSMITTED_ACK){}
    }
    return (I2C_TWSR_FLAG_MASK) != DATA_BYTE_TRANSMITTED_ACK;
 }


 /*==============================================================================
 *  SECTION: READ  / RX  (POLLING)
 *------------------------------------------------------------------------------
 *  Purpose: Routines that *read* bytes/buffers from the I²C bus in polling mode.
 *  Includes: i2cReadPol(), i2cReadPOL_(), <future read helpers>.
 *==============================================================================*/

 uint8_t i2cReadPol(char *buffer, size_t size, uint8_t address){
    /*Sanity Check*/
    if(buffer == NULL) return 0x01;
    
    /*Mode Adjustment*/
    if(i2cPolHelper(address, I2C_READ) != SLA_PLUS_W_ACK){
        twsrFlagHandler();
    }
    else{
        for(size_t i=0; i<size; i++){
        while(!(TWCR & (1<<TWINT)));                      // Wait for Hardware flag
        TWDR = buffer[i];                             // Write data to I2C register
        TWCR = (1<<TWINT) | (1<<TWEN);                    // Send data
        while((TWSR & I2C_TWSR_FLAG_MASK) != DATA_BYTE_TRANSMITTED_ACK){} // Wait for hardware flag
        }
    }
    
    TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN);          // Send a Stop condition 
    return TWSR & I2C_TWSR_FLAG_MASK;                                
}

uint8_t i2cReadPol_(char *buffer, size_t size){
    if(buffer == NULL) return 0x01;
    for(size_t i=0; i<size; i++){
        while(!(TWCR & (1<<TWINT)));
        TWDR = buffer[i];
        TWCR = (1<<TWINT) | (1<<TWEN); 
        while((TWSR & I2C_TWSR_FLAG_MASK) != DATA_BYTE_TRANSMITTED_ACK){}
    }
    return (TWSR & I2C_TWSR_FLAG_MASK) != DATA_BYTE_TRANSMITTED_ACK;
 }


 