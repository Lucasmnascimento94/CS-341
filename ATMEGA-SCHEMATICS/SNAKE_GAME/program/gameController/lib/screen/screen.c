#include  "screen.h"
#include "string.h"

void screenDefault(SCREEN *screen){
    screen->conf->I_D = 1;
    screen->conf->SH = 0;
    screen->conf->D = 1;
    screen->conf->C = 1;
    screen->conf->B = 1;
    screen->conf->S_C = 0;
    screen->conf->R_L = 1;
    screen->conf->DL = 0;
    screen->conf->N = 1;
    screen->conf->F = 0;

    screen->current_column = 0;
    screen->current_row = 0X4E >> 1;
}

uint8_t *buildInstrucion(uint16_t command){
    static uint8_t instruction[5];
    memset(instruction, 0, 5);

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

    instruction[0] = hi_nibble | control_nibble;
    instruction[1] = instruction[0] & ~(1<<E);
    instruction[2] = lo_nibble | control_nibble;
    instruction[3] = instruction[2] & ~(1<<E);
    instruction[4] = '\0';

    return instruction;
}

void screenInit(SCREEN *screen){
    screenDefault(screen);
    i2cWritePol(\
        (char *)buildInstrucion(FUNCTION_SET(screen->conf->DL, screen->conf->N, screen->conf->F)), 4, screen->pcf8574_addr);
        _delay_us(100);

    i2cWritePol(\
        (char *)buildInstrucion(FUNCTION_SET(screen->conf->DL, screen->conf->N, screen->conf->F)), 4, screen->pcf8574_addr);
        _delay_us(100);
    
    i2cWritePol((char *)buildInstrucion(CLEAR_DISPLAY), 4, screen->pcf8574_addr);
    _delay_us(100);

    i2cWritePol(\
        (char *)buildInstrucion(ENTRY_MODE(screen->conf->I_D, screen->conf->SH)), 4, screen->pcf8574_addr);
        _delay_us(100);

    i2cWritePol(\
        (char *)buildInstrucion(DISPLAY_ON_OFF(screen->conf->D, screen->conf->C, screen->conf->B)), 4, screen->pcf8574_addr);
        _delay_us(100);

    i2cWritePol(\
        (char *)buildInstrucion(CURSOR(screen->conf->S_C, screen->conf->R_L)), 4, screen->pcf8574_addr);
        _delay_us(100);

    i2cWritePol(\
        (char *)buildInstrucion(HOME), 4, screen->pcf8574_addr);
        _delay_us(100);
}

uint8_t newAddrLine4(SCREEN *screen){
    static uint8_t addr = 0;
    uint8_t current = screen->current_column;

    switch (screen->current_row){
        case 0:
            addr = current+1;
            break;
        case 1:
            addr = 0x40 + current + 1;
            break;
        case 2:
            addr = 0x14 + current + 1;
            break;
        case 3:
            addr = 0x54 + current + 1;
            break;
    }
    screen->current_column = (current < 19)? current+ 1: 0;
    screen->current_row= (current < 3)? screen->current_row+ 1: 0;
    return addr;
}

uint8_t newAddrLine2(SCREEN *screen){
    static uint8_t addr = 0;
    uint8_t current = screen->current_column;

    switch (screen->current_row){
        case 0:
            addr = current+1;
            break;
        case 1:
            addr = 0x14 + current + 1;
            break;
    }
    screen->current_column = (current < 19)? current+ 1: 0;
    screen->current_row= (current < 1)? screen->current_row+ 1: 0;
    return addr;
}

void writeBytes(char *buffer){
    // Fixed: 'static' makes this memory permanent (non-dangling)
    static char instruction[5] = {0}; 
    uint8_t control_nibble = (1 << RS_BIT); // <-- FIXED: Set RS=1
    uint16_t size = strlen(buffer);

    for(uint16_t i=0; i<size; i++){
        // This calculates the control byte *with* E=1
        uint8_t control_e_high = control_nibble | (1<<BT) | (1<<E); 
        
        uint8_t hi_nibble = (buffer[i] & 0xF0);
        uint8_t lo_nibble = ((buffer[i] & 0x0F) << 4);

        instruction[0] = hi_nibble | control_e_high;
        instruction[1] = instruction[0] & ~(1<<E); // E=0
        instruction[2] = lo_nibble | control_e_high;
        instruction[3] = instruction[2] & ~(1<<E); // E=0
        instruction[4] = '\0'; // Not necessary for I2C transfer
        
        // Send the 4 bytes for one character
        i2cWritePol_(instruction, 4);
        _delay_us(60); 
    }
}

void screenWrite(SCREEN *screen, char *buffer){
    if(buffer == NULL) return;

    // Set address to start writing on DDRAM
    uint8_t next_addr = (screen->conf->N)?newAddrLine4(screen):newAddrLine2(screen);
    i2cWritePol((char *)buildInstrucion(((1<<RAM_BIT) | next_addr)), 4, screen->pcf8574_addr);
    _delay_us(100);

    i2cStartPol(screen->pcf8574_addr, I2C_WRITE); // Start protocl with no auto control
    _delay_us(1);
    writeBytes(buffer);
    i2cStop();
}