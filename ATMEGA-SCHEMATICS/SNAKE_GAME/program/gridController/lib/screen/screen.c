#include  "screen.h"
#include "uart.h"
#include "string.h"
#include "stdio.h"
char c[50];
void buildCommand(SCREEN_CONF *screen, uint16_t command){
    memset(screen->instruction, 0, 6);
    uint8_t control_nibble = 0x00;
    if(command != READ_BUSY_FLAG && \
       command != WRITE_TO_RAM && \
       command != READ_FROM_RAM)
    {control_nibble = 0x00;}
    else{
        switch (command){
            case READ_BUSY_FLAG:
                control_nibble |= (1<<R_W_BIT);
                break;
            case WRITE_TO_RAM:
                uartWrite_("IT SHOULD HERE\n");
                control_nibble |= (1<<RS_BIT);
                control_nibble &= ~(1<<R_W_BIT);
                break;
            case READ_FROM_RAM:
                control_nibble |= (1<<RS_BIT) | (1<<R_W_BIT);
                break;
            default:
                break;
        }
    }

    control_nibble |= (1<<BT) | (1<<E);
    uint8_t hi_nibble = (((uint8_t)command) & 0xF0);
    uint8_t lo_nibble = ((((uint8_t)command) & 0x0F) << 4);

    screen->instruction[0] = hi_nibble | control_nibble;
    screen->instruction[1] = screen->instruction[0] & ~(1<<E);
    _delay_ms(2);
    screen->instruction[2] = lo_nibble | control_nibble;
    screen->instruction[3] = screen->instruction[2] & ~(1<<E);
    screen->instruction[4] = '\0';
}

void screenInit(I2C_CONF *i2c_conf, I2C_SLAVE *i2c_slave, SCREEN_CONF *screen_conf){
    // Change the screen to 4 bit mode operatiyeson
    i2c_conf->slave = malloc(sizeof(I2C_SLAVE*)*i2c_slave->count);
    i2c_conf->slave[PCF8574_INDEX] = i2c_slave;


    buildCommand(screen_conf, FUNCTION_SET);
    i2c_conf->data = screen_conf->instruction;
    i2cMasterWrite_POL(i2c_conf, PCF8574_INDEX);
    _delay_us(100);

    buildCommand(screen_conf, FUNCTION_SET);
    i2c_conf->data = screen_conf->instruction;
    i2cMasterWrite_POL(i2c_conf, PCF8574_INDEX);
    _delay_us(100);

    buildCommand(screen_conf, CLEAR_DISPLAY);
    i2c_conf->data = screen_conf->instruction;
    i2cMasterWrite_POL(i2c_conf, PCF8574_INDEX);
    _delay_ms(3);

    buildCommand(screen_conf, ENTRY_MODE);
    i2c_conf->data = screen_conf->instruction;
    i2cMasterWrite_POL(i2c_conf, PCF8574_INDEX);
    _delay_us(100);

    buildCommand(screen_conf, DISPLAY_ON_OFF);
    i2c_conf->data = screen_conf->instruction;
    i2cMasterWrite_POL(i2c_conf, PCF8574_INDEX);
    _delay_us(100);

    buildCommand(screen_conf, CURSOR);
    i2c_conf->data = screen_conf->instruction;
    i2cMasterWrite_POL(i2c_conf, PCF8574_INDEX);
    _delay_us(100);

    buildCommand(screen_conf, HOME);
    i2c_conf->data = screen_conf->instruction;
    i2cMasterWrite_POL(i2c_conf, PCF8574_INDEX);
    _delay_ms(3);

    memset(screen_conf->instruction, 0, 6);
}

void writeBytes(I2C_CONF *i2c_conf){
    // Fixed: 'static' makes this memory permanent (non-dangling)
    static char instruction[5] = {0}; 
    uint8_t control_nibble = (1 << RS_BIT); // <-- FIXED: Set RS=1
    char *temp = i2c_conf->data; // User string pointer is saved
    uint16_t size = strlen(i2c_conf->data);
    for(uint16_t i=0; i<size; i++){
        
        // This calculates the control byte *with* E=1
        uint8_t control_e_high = control_nibble | (1<<BT) | (1<<E); 
        
        uint8_t hi_nibble = (temp[i] & 0xF0);
        uint8_t lo_nibble = ((temp[i] & 0x0F) << 4);

        instruction[0] = hi_nibble | control_e_high;
        instruction[1] = instruction[0] & ~(1<<E); // E=0
        instruction[2] = lo_nibble | control_e_high;
        instruction[3] = instruction[2] & ~(1<<E); // E=0
        instruction[4] = '\0'; // Not necessary for I2C transfer

        // Set the I2C pointer to the 4-byte instruction buffer
        i2c_conf->data = instruction;
        
        // Send the 4 bytes for one character
        i2cMasterWrite_POL_SEND(i2c_conf, PCF8574_INDEX);
        _delay_us(60); 
    }
}

void screenWrite(I2C_CONF *i2c_conf, SCREEN_CONF *screen_conf){
    if(i2c_conf->data == NULL) return;

    char *temp = i2c_conf->data;
    buildCommand(screen_conf, ((DDRAM<<RAM_BIT) | 0X00));
    i2c_conf->data = screen_conf->instruction;
    i2cMasterWrite_POL(i2c_conf, PCF8574_INDEX);
    _delay_us(100);

    buildCommand(screen_conf, WRITE_TO_RAM);
    i2c_conf->data = screen_conf->instruction;
    i2cMasterWrite_POL_START(i2c_conf, PCF8574_INDEX);
    i2cMasterWrite_POL_SEND(i2c_conf, PCF8574_INDEX);
    i2c_conf->data = temp;
    writeBytes(i2c_conf);
    i2cMasterWrite_POL_STOP(i2c_conf, PCF8574_INDEX);
    memset(screen_conf->instruction, 0, 6);
}
