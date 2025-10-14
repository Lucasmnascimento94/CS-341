#include  "screen.h"
#include "uart.h"
char c[50];

void sendCommand(I2C_CONF *i2c_conf, I2C_SLAVE *i2c_slave,  uint8_t command){
    uartWrite_("called send command\n");
    char instruction[5];
    uint8_t byte;
    if(command != READ_BUSY_FLAG && \
       command != WRITE_TO_RAM && \
       command != READ_FROM_RAM)
    {byte = 0x00;}
    else{
        return;
    }
    instruction[0] = byte | (1<<BT);
    byte |= (1<<E);
    instruction[1] = byte;
    instruction[2] |= (command & 0xF0) | (byte);
    instruction[3] |= ((command & 0xF) << 4) | (byte);
    byte &= ~(1<<E);
    instruction[4] = byte;

    i2c_conf->data = instruction;
    i2cMasterWrite_POL(i2c_conf, PCF8574_INDEX);
    uartWrite_("leaving send command\n");
}

void screenInit(I2C_CONF *i2c_conf, I2C_SLAVE *i2c_slave, SCREEN_CONF *screen_conf){
    // Change the screen to 4 bit mode operation
    i2c_conf->slave = malloc(sizeof(I2C_SLAVE)*i2c_slave->count);
    i2c_conf->slave[PCF8574_INDEX] = i2c_slave;

    sprintf(c, "THIS IS ADDR IN CONF: %X\n", i2c_conf->slave[PCF8574_INDEX]->addr);
    uartWrite_(c);

    sendCommand(i2c_conf, i2c_slave, FUNCTION_SET);
    _delay_us(60);

    sendCommand(i2c_conf, i2c_slave, FUNCTION_SET);
    _delay_us(60);

    sendCommand(i2c_conf, i2c_slave, CLEAR_DISPLAY);
    _delay_us(60);

    sendCommand(i2c_conf, i2c_slave, ENTRY_MODE);
    _delay_us(60);

    sendCommand(i2c_conf, i2c_slave, DISPLAY_ON_OFF);
    _delay_us(60);

    sendCommand(i2c_conf, i2c_slave, CURSOR);
    _delay_us(60);

    sendCommand(i2c_conf, i2c_slave, HOME);
    _delay_us(60);
}

void screenWrite(I2C_CONF *i2c_conf, SCREEN_CONF *screen_conf, uint8_t line){
    
}
