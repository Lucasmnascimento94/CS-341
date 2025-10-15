#include "i2c.h"
#include "string.h"
#include "uart.h"
#include "stdio.h"

void i2cConfMaster_POL();
void i2cConfMaster_INT();
void i2cConfSlave_POL();
void i2cConfSlave_INT();
 uint8_t i2cWrite_POL_Helper(I2C_PORT *port, I2C_TARGET *target);
 void twsrFlagHandler(I2C_TARGET *target, char *msg);
/*==============================================================================
 *  I2C Clock Configuration
 *==============================================================================*/
void i2cClockConfig(I2C_TARGET *target){
    /*Note:
    Slave operation does not depend on Bit Rate or Prescaler settings, but the CPU clock 
    frequency in the Slave must be at least 16 times higher than the SCL frequency. 
    */
    
        switch (target->prescaler){
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
    
/*#######################################___I2C INITIALIZERS___#################################*/
/*==============================================================================
 *  I2C Mode Configuration
 *==============================================================================*/
void i2cModeConf(I2C_TARGET *target){
    switch (target->mode){
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
 *  i2cConf Helpers
 *==============================================================================*/
void i2cConfMaster_POL(){
    /*to do*/
}

void i2cConfMaster_INT(){
    /*to do*/
}

void i2cConfSlave_POL(){
    /*to do*/
}

void i2cConfSlave_INT(){
    /*to do*/
}

/*==============================================================================
    *  Transmit Start in Polling Mode and return the status
*==============================================================================*/
uint8_t start_POL(I2C_TARGET *target){
    /* Modify address byte on I2C protocol (SLA+W)
       Writing::direction==0 | Reading::direction==1*/
    uint8_t address = (target->addr << 1) | (target->direction & 0x01);

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
    //uint8_t address = (target->addr << 1) | (target->direction & 0x01);       // Modify address byte on I2C protocol (SLA+W)

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
    if(port == NULL || port->data == NULL || target == NULL) return 0x01;
    
    /*Mode Adjustment*/
    i2cWrite_POL_Helper(port, target);
    if((TWSR & I2C_TWSR_FLAG_MASK) != SLA_PLUS_W_ACK){
        twsrFlagHandler(target, "i2cWrite_Pol");
    }
    else{
        for(size_t i=0; i<port->data_size; i++){
        while(!(TWCR & (1<<TWINT)));                      // Wait for Hardware flag
        TWDR = port->data[i];                             // Write data to I2C register
        TWCR = (1<<TWINT) | (1<<TWEN);                    // Send data
        while(TWSR != DATA_BYTE_TRANSMITTED_ACK){} // Wait for hardware flag
    }
    }
    
    TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN);          // Send a Stop condition 
    return TWSR & I2C_TWSR_FLAG_MASK;  ;                              
 }

 uint8_t i2cWrite_POL_Helper(I2C_PORT *port, I2C_TARGET *target){

    if(port->current_mode != target->mode) i2cModeConf(target);               // Update Mode if needed
    if(port->current_target_addr != target->addr) i2cClockConfig(target);   // Adjust clock based on the target if neeeded

    uint8_t tries = 0;                                                      // Counter
    uint8_t status = 0;                                                     // status flag

    for(;status != SLA_PLUS_W_ACK; tries++){                                // Check for flag in polling mode
        if(tries>=10) return status;                                        // Return if it failed 10 times.
        status = start_POL(target);                                         // Start I2C protocol and wait for ACK
        
        if(status == SLA_PLUS_W_ACK) break;                                 // break loop if ACK is received
        TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN);                        // Reset and send STOP signal, to start over.
        _delay_us(1);
    }
    return status;
 }

/*==============================================================================
 * Send data buffer in port to the target
 - This function gives Control of START/STOP to the caller
 *==============================================================================*/
uint8_t i2cWriteNoCtrl_POL(I2C_PORT *port){
    for(size_t i=0; i<port->data_size; i++){
        while(!(TWCR & (1<<TWINT)));
        TWDR = port->data[i];
        TWCR = (1<<TWINT) | (1<<TWEN); 
        while(TWSR != DATA_BYTE_TRANSMITTED_ACK){}
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


/*#######################################___I2C FLAG HANDLERS___#################################*/

void twsrFlagHandler(I2C_TARGET *target, char *msg){
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