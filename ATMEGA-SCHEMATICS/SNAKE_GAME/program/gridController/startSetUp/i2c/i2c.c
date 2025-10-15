#include "i2c.h"
#include "string.h"
#include "uart.h"
#include "stdio.h"


char flag[40];

/*#######################################___I2C INITIALIZERS___#################################*/
/*==============================================================================
 *  I2C Initial Configuration
 *==============================================================================*/
uint8_t i2cConf(I2C_PORT *port, I2C_TARGET *target){
    /*Sanity Check*/
    switch (target->mode){
        case MODE_MASTER_POL:
            break;
        case MODE_MASTER_INT:
            break;
        case MODE_SLAVE_POL:
            break;
        case MODE_SLAVE_INT:
            break;
        default:
            uartWrite_("Err..i2cConf<Invalid Mode>");
            return 0x01;
    }
}

uint8_t i2cConf_

/*==============================================================================
 *  I2C Clock Configuration
 *==============================================================================*/
void i2cClockConfig(I2C_TARGET *target){
    /*Note:
    Slave operation does not depend on Bit Rate or Prescaler settings, but the CPU clock 
    frequency in the Slave must be at least 16 times higher than the SCL frequency. 
    */
        conf->frequency = F_CPU/16;
    
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
    
        uint32_t twbr_num = (F_CPU / target->frequency);
        uint32_t twbr_den = 2*(target->prescaler);
        TWBR = (uint8_t) ((twbr_num - 16)/twbr_den);
    }
    
/*==============================================================================
    *  Transmit Start in Polling Mode and return the status
*==============================================================================*/
uint8_t start_POL(I2C_TARGET *target){
    uint8_t mode = target->mode & 0x01;                 // Polling::0 | Interrupt::1
    uint8_t address = (target->addr << 1) | mode;       // Modify address byte on I2C protocol (SLA+W)

    TWCR |= (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);        // Send Start Signal on SDA bus
    while(!(TWCR & (1<<TWINT)));                        // Check for hardware feedback

    TWDR=address;                                       // Write SLA+W onto data register
    TWCR &= ~(1<<TWSTA);                                // Clear STA bit
    TWCR |= (1<<TWINT) | (1<<TWEN);                     // Clear flag
    while(!(TWCR & (1<<TWINT)));                        // Wait for hardware flag

    return TWSR & I2C_TWSR_FLAG_MASK;                   // Return status flag
}

/*==============================================================================
    *  Transmit Start in Iterrupt Mode and return the status
*==============================================================================*/
uint8_t start_INT(I2C_TARGET *target){
    uint8_t mode = target->mode & 0x01;                 // Polling::0 | Interrupt::1
    uint8_t address = (target->addr << 1) | mode;       // Modify address byte on I2C protocol (SLA+W)

    return TWSR & I2C_TWSR_FLAG_MASK;                   // Return status flag
}

/*==============================================================================
 * Helper function to Stop the protocol
 *==============================================================================*/
uint8_t i2cSTOP(){
    TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN); // Send a Stop condition*/
    return TWSR & I2C_TWSR_FLAG_MASK; 
}



/*#######################################___I2C WRITING FUNCTIONS___#################################*/

/*==============================================================================
 * Send data buffer in port to the target
 *==============================================================================*/
 uint8_t i2cWrite_POL(I2C_PORT *port, I2C_TARGET *target){
    /*Sanity Check*/
    if(port == NULL || port->data == NULL || target == NULL) return;

    uint8_t tries = 0;                                    // Counter
    uint8_t status = 0;                                   // status flag
    if(conf->data == NULL)return;


    for(;status != SLA_PLUS_W_ACK; tries++){              // Check for flag in polling mode
        if(tries>=10) return status;                      // Return if it failed 10 times.
        status = start_POL(target);                 // Start I2C protocol and wait for ACK
        
        if(status == SLA_PLUS_W_ACK) break;               // break loop if ACK is received
        TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN);      // Reset and send STOP signal, to start over.
        _delay_us(100);
    }


    for(size_t i=0; i<port->data_size; i++){
        while(!(TWCR & (1<<TWINT)));                      // Wait for Hardware flag
        TWDR = conf->data[i];                             // Write data to I2C register
        TWCR = (1<<TWINT) | (1<<TWEN);                    // Send data
        while(TWSR != DATA_BYTE_TRANSMITTED_ACK){TWDR = conf->data[i];} // Wait for hardware flag
    }
    
    TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN);          // Send a Stop condition 
    return TWSR & I2C_TWSR_FLAG_MASK;  ;                              
 }

/*==============================================================================
 * Send data buffer in port to the target
 - This function gives Control of START/STOP to the caller
 *==============================================================================*/
uint8_t i2cWriteNoCtrl_POL(I2C_PORT *port, I2C_TARGET *target){
    for(size_t i=0; i<port->data_size; i++){
        while(!(TWCR & (1<<TWINT)));
        TWDR = port->data[i];
        TWCR = (1<<TWINT) | (1<<TWEN); 
        while(TWSR != DATA_BYTE_TRANSMITTED_ACK){TWDR = conf->data[i];}
    }
 }




/*#######################################___I2C READING FUNCTIONS___#################################*/

/*==============================================================================
    * Send data buffer in port to the target
*==============================================================================*/
uint8_t i2cRead_POL(I2C_PORT *port, I2C_TARGET *target){
/*Sanity Check*/
if(port == NULL || port->data == NULL || target == NULL) return;

TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN);          // Send a Stop condition 
return TWSR & I2C_TWSR_FLAG_MASK;  ;                              
}

/*==============================================================================
    * Send data buffer in port to the target
    - This function gives Control of START/STOP to the caller
*==============================================================================*/
uint8_t i2cReadNoCtrl_POL(I2C_PORT *port, I2C_TARGET *target){
}
