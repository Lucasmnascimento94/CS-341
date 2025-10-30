#include  "screen.h"
#include "string.h"

void screenDefault(SCREEN *screen){
    screen->conf.I_D = 1;
    screen->conf.SH = 0;
    screen->conf.D = 1;
    screen->conf.C = 0;
    screen->conf.B = 0;
    screen->conf.S_C = 0;
    screen->conf.R_L = 1;
    screen->conf.DL = 0;
    screen->conf.N = 1;
    screen->conf.F = 0;

    screen->current_column = 0;
    screen->current_row = 0;
    screen->pcf8574_addr = 0x4E>>1;
}

uint8_t *buildInstruction(uint16_t command){
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

    control_nibble |= (1<<BT_BIT) | (1<<E_BIT);
    uint8_t hi_nibble = (((uint8_t)command) & 0xF0);
    uint8_t lo_nibble = ((((uint8_t)command) & 0x0F) << 4);

    instruction[0] = hi_nibble | control_nibble;
    instruction[1] = instruction[0] & ~(1<<E_BIT);
    instruction[2] = lo_nibble | control_nibble;
    instruction[3] = instruction[2] & ~(1<<E_BIT);
    instruction[4] = '\0';

    return instruction;
}

void screenInit(SCREEN *screen, bool default_conf){
    if(default_conf) screenDefault(screen);
    i2cWritePol(\
        (char *)buildInstrucion(FUNCTION_SET(screen->conf.DL, screen->conf.N, screen->conf.F)), 4, screen->pcf8574_addr);
        _delay_us(100);

    i2cWritePol(\
        (char *)buildInstrucion(FUNCTION_SET(screen->conf.DL, screen->conf.N, screen->conf.F)), 4, screen->pcf8574_addr);
        _delay_us(100);
    
    i2cWritePol((char *)buildInstrucion(CLEAR_DISPLAY), 4, screen->pcf8574_addr);
    _delay_ms(2);

    i2cWritePol(\
        (char *)buildInstrucion(ENTRY_MODE(screen->conf.I_D, screen->conf.SH)), 4, screen->pcf8574_addr);
        _delay_us(100);

    i2cWritePol(\
        (char *)buildInstrucion(DISPLAY_ON_OFF(screen->conf.D, screen->conf.C, screen->conf.B)), 4, screen->pcf8574_addr);
        _delay_us(100);

    i2cWritePol(\
        (char *)buildInstrucion(CURSOR(screen->conf.S_C, screen->conf.R_L)), 4, screen->pcf8574_addr);
        _delay_us(100);

    i2cWritePol(\
        (char *)buildInstrucion(HOME), 4, screen->pcf8574_addr);
        _delay_ms(2);
}


uint8_t newAddrLine4(SCREEN *screen){
    uint8_t current_column = screen->current_column;

    if(current_column < 20){
        screen->current_column++;
    }
    else{
        current_column = 0;
        screen->current_row = (screen->current_row<3)?screen->current_row + 1:0;
        screen->current_column = 1;
    }

    return getAddress(screen->current_row, current_column);
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

uint8_t getAddress(uint8_t row, uint8_t column){
    uint8_t base = 0;
    switch (row){
        case 0: base = 0x00; break;
        case 1: base = 0x40; break;
        case 2: base = 0x14; break;
        case 3: base = 0x54; break;
    }

    return base + column;
}

void buildBytes(uint8_t *buffer, uint8_t byte){
    // Fixed: 'static' makes this memory permanent (non-dangling)
    if(buffer == NULL) return;
    uint8_t control_nibble = (1 << RS_BIT); // <-- FIXED: Set RS=1

    // This calculates the control byte *with* E=1
    uint8_t control_e_high = control_nibble | (1<<BT_BIT) | (1<<E_BIT) | (1<<RS_BIT); 
    
    uint8_t hi_nibble = (byte & 0xF0);
    uint8_t lo_nibble = ((byte & 0x0F) << 4);

    buffer[0] = hi_nibble | control_e_high;
    buffer[1] = buffer[0] & ~(1<<E_BIT); // E=0
    buffer[2] = lo_nibble | control_e_high;
    buffer[3] = buffer[2] & ~(1<<E_BIT); // E=0
    buffer[4] = '\0'; // Not necessary for I2C transfer
}


void setCursor(uint8_t pos, uint8_t pcf_address){
    i2cWritePol((char *)buildInstrucion(((1<<RAM_BIT) | pos)), 4, pcf_address); // Set Address
    _delay_us(100);
}

void screenWrite(SCREEN *screen, char *buffer){
    if(buffer == NULL) return;

    for(uint16_t i=0; i<strlen(buffer); i++){
        uint8_t next_addr = (screen->conf.N)?newAddrLine4(screen):newAddrLine2(screen);
        setCursor(next_addr, screen->pcf8574_addr); // Set Address

        // This calculates the control byte *with* E=1
        uint8_t bytes[5] = {0};
        buildBytes(bytes, (uint8_t)buffer[i]);
        i2cWritePol((char *)bytes, 4, screen->pcf8574_addr);
        _delay_us(45); 
    }
}

void screenClear(SCREEN *screen) {
    i2cWritePol((char *)buildInstrucion(CLEAR_DISPLAY), 4, screen->pcf8574_addr);
    _delay_ms(2);
    screen->current_column = 0;
    screen->current_row = 0;
}

void screenSetCursor(SCREEN *screen, uint8_t row, uint8_t column) {
    screen->current_row = row;
    screen->current_column = column;
    uint8_t addr = getAddress(row, column);
    setCursor(addr, screen->pcf8574_addr);
}

void screenWriteAt(SCREEN *screen, uint8_t row, uint8_t column, char *text) {
    screenSetCursor(screen, row, column);
    screenWrite(screen, text);
}

void updateGameScreen(SCREEN *screen, const char *game_name, uint8_t top_score, uint8_t current_score) {
    char top_str[10];
    snprintf(top_str, sizeof(top_str), "Top Score: %-3d", top_score);

    screenWriteAt(screen, 0, 0, (char *)game_name); 
    screenWriteAt(screen, 0, 10, top_str);

    char score_str[20];
    snprintf(score_str, sizeof(score_str), "Current score: %-3d", current_score);
    screenWriteAt(screen, 3, 0, score_str);
}

