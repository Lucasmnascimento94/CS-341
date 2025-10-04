#include "opcodes/sram.h"
#include "string.h"

#include <util/twi.h>

#define USE_I2C 1
#define I2C_SLAVE_ADDRESS 0xA0
#define F_CPU 16000000UL
#define SCL_CLOCK 100000L

#if USE_I2C

// Implement I2C Protocol.
void i2c_init(void) {
    TWSR = 0;
    TWBR = ((F_CPU / SCL_CLOCK) - 16) / 2;
}

void i2c_begin(void) {
    TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);
    while (!(TWCR & (1<<TWINT)));
}

void i2c_end(void) {
    TWCR = (1<<TWSTO)|(1<<TWEN)|(1<<TWINT);
}

void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1<<TWEN)|(1<<TWINT);
    while (!(TWCR & (1<<TWINT)));
}

#endif

// Use either I2C or SPI. For now, use I2C.
void writeByte(char *c, uint32_t address) {
#if USE_I2C
    i2c_begin();
    i2c_write(I2C_SLAVE_ADDRESS);
    for (uint8_t i = 0; i < strlen(c); i++){
        i2c_write((uint8_t)c[i]);          
    }
    i2c_end();
#else
    START_SPI;                         // CS low
    for (uint8_t i = 0; i < strlen(c); i++){
        SPDR = (uint8_t)c[i];          // start transfer
    }
    STOP_SPI;                          // CS high
#endif
}