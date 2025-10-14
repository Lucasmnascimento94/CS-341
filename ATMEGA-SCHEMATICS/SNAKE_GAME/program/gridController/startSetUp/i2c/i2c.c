#include "i2c.h"
#include "string.h"
#include "uart.h"
#include "stdio.h"


char flag[40];
/*==============================================================================
 *  I2C Initial Configuration
 *==============================================================================*/
void i2cConfig(I2C_CONF *conf){
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

    uint32_t twbr_num = (F_CPU / conf->frequency);
    uint32_t twbr_den = 2*(conf->prescaler);
    TWBR = (uint8_t) ((twbr_num - 16)/twbr_den);
}

/*==============================================================================
 *  Transmit Start in Polling Mode and return the status
 *==============================================================================*/
uint8_t masterStart_POL(I2C_CONF *conf, uint8_t slave_index){
    uint8_t address = (conf->slave[slave_index]->addr << 1) | conf->read_mode;
    TWCR |= (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

    while(!(TWCR & (1<<TWINT)));
    TWDR=address;
    //TWDR = (conf->slave[slave_index]->addr << 1) & ~(conf->read_mode); // Send the Slave address in the data bus to connect to the specific slave
    TWCR &= ~(1<<TWSTA); // Clear STA bit
    TWCR |= (1<<TWINT) | (1<<TWEN); // Clear flag
    while(!(TWCR & (1<<TWINT)));

    sprintf(flag, "This is status: %X\n", TWSR & I2C_TWSR_FLAG_MASK);
    uartWrite_(flag);
    return TWSR & I2C_TWSR_FLAG_MASK;
}

 void i2cMasterWrite_POL(I2C_CONF *conf, uint8_t slave_index){
    uint8_t tries = 0;
    uint8_t status = 0;
    if(conf->data == NULL)return;

    while(status != SLA_PLUS_W_ACK && tries++<10){
        uartWrite_("Search for Target\n");
        status = masterStart_POL(conf, slave_index);
        if(status == SLA_PLUS_W_ACK) break;
        sprintf(flag, "%X\n", status);
        uartWrite_(flag);
        TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN);
        _delay_ms(1);
    }

    if(tries >= 10) return;
    for(uint16_t i=0; i<strlen(conf->data); i++){
        while(!(TWCR & (1<<TWINT)));
        TWDR = conf->data[i];
        TWCR = (1<<TWINT) | (1<<TWEN); 
        while(TWSR != DATA_BYTE_TRANSMITTED_ACK){TWDR = conf->data[i];}
    }
    
    TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN); // Send a Stop condition*/
    conf->data = NULL;
 }


void i2cMasterWrite_POL_START(I2C_CONF *conf, uint8_t slave_index){
    uartWrite_("START 1\n");
    uint8_t tries = 0;
    uint8_t status = 0;
    if(conf->data == NULL)return;
    uartWrite_("START 2\n");
    while(status != SLA_PLUS_W_ACK && tries++<10){
        uartWrite_("Search for Target\n");
        status = masterStart_POL(conf, slave_index);
        if(status == SLA_PLUS_W_ACK) break;
        sprintf(flag, "%X\n", status);
        uartWrite_(flag);
        TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN);
        _delay_ms(1);
    }

    if(tries >= 10) return;
    sprintf(flag, "size: %d\n", strlen(conf->data));
    uartWrite_(flag);
 }
void i2cMasterWrite_POL_SEND(I2C_CONF *conf, uint8_t slave_index){
    uartWrite_("START SEND\n");
    for(uint16_t i=0; i<strlen(conf->data); i++){
        while(!(TWCR & (1<<TWINT)));
        TWDR = conf->data[i];
        TWCR = (1<<TWINT) | (1<<TWEN); 
        while(TWSR != DATA_BYTE_TRANSMITTED_ACK){TWDR = conf->data[i];}
    }
 }
void i2cMasterWrite_POL_STOP(I2C_CONF *conf, uint8_t slave_index){
    uartWrite_("STOP\n");
    TWCR |= (1<<TWSTO) | (1<<TWINT) | (1<<TWEN); // Send a Stop condition*/
    conf->data = NULL;
}
 /*void i2cMasterRead_POL(I2C_CONF *conf, char *data, int size){

 }*/