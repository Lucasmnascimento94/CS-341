#include  "screen.h"
#include "uart.h"
#include "string.h"
#include "stdio.h"
char c[50];
void buildCommand(I2C_PORT *port, uint16_t command){
    memset(port->instruction, 0, port->intruction_size);
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

    port->instruction[0] = hi_nibble | control_nibble;
    port->instruction[1] = port->instruction[0] & ~(1<<E);
    _delay_ms(2);
    port->instruction[2] = lo_nibble | control_nibble;
    port->instruction[3] = port->instruction[2] & ~(1<<E);
    port->instruction[4] = '\0';
}

void screenInit(I2C_PORT *port, I2C_TARGET *target){
    // Change the screen to 4 bit mode operatiyeson

    buildCommand(port, FUNCTION_SET);
    port->data = port->instruction;
    i2cWrite_POL(port, target);
    _delay_us(100);

    buildCommand(port, FUNCTION_SET);
    port->data = port->instruction;
    i2cWrite_POL(port, target);
    _delay_us(100);

    buildCommand(port, CLEAR_DISPLAY);
    port->data = port->instruction;
    i2cWrite_POL(port, target);
    _delay_ms(3);

    buildCommand(port, ENTRY_MODE);
    port->data = port->instruction;
    i2cWrite_POL(port, target);
    _delay_us(100);

    buildCommand(port, DISPLAY_ON_OFF);
    port->data = port->instruction;
    i2cWrite_POL(port, target);
    _delay_us(100);

    buildCommand(port, CURSOR);
    port->data = port->instruction;
    i2cWrite_POL(port, target);
    _delay_us(100);

    buildCommand(port, HOME);
    port->data = port->instruction;
    i2cWrite_POL(port, target);
    _delay_ms(3);

    memset(port->instruction, 0, 6);
}

void writeBytes(I2C_PORT *port){
    // Fixed: 'static' makes this memory permanent (non-dangling)
    static char instruction[5] = {0}; 
    uint8_t control_nibble = (1 << RS_BIT); // <-- FIXED: Set RS=1
    char *temp = port->data; // User string pointer is saved
    uint16_t size = strlen(port->data);
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
        port->data = instruction;
        
        // Send the 4 bytes for one character
        i2cWriteNoCtrl_POL(port);
        _delay_us(60); 
    }
}

void screenWrite(I2C_PORT *port, I2C_TARGET *target, SCREEN_CONF *screen){
    if(port->data == NULL) return;

    char *temp = port->data;
    buildCommand(port, ((DDRAM<<RAM_BIT) | 0X00));
    port->data = port->instruction;
    i2cWrite_POL(port, target);
    _delay_us(100);

    buildCommand(port, WRITE_TO_RAM);
    port->data = port->instruction;
    start_POL(target);
    i2cWriteNoCtrl_POL(port);
    port->data = temp;
    writeBytes(port);
    i2cSTOP();
    memset(port->instruction, 0, 6);
}
