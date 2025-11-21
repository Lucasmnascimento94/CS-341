#include  "screen.h"

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
        (char *)buildInstruction(FUNCTION_SET(screen->conf.DL, screen->conf.N, screen->conf.F)), 4, screen->pcf8574_addr);
        _delay_us(100);

    i2cWritePol(\
        (char *)buildInstruction(FUNCTION_SET(screen->conf.DL, screen->conf.N, screen->conf.F)), 4, screen->pcf8574_addr);
        _delay_us(100);
    
    i2cWritePol((char *)buildInstruction(CLEAR_DISPLAY), 4, screen->pcf8574_addr);
    _delay_ms(2);

    i2cWritePol(\
        (char *)buildInstruction(ENTRY_MODE(screen->conf.I_D, screen->conf.SH)), 4, screen->pcf8574_addr);
        _delay_us(100);

    i2cWritePol(\
        (char *)buildInstruction(DISPLAY_ON_OFF(screen->conf.D, screen->conf.C, screen->conf.B)), 4, screen->pcf8574_addr);
        _delay_us(100);

    i2cWritePol(\
        (char *)buildInstruction(CURSOR(screen->conf.S_C, screen->conf.R_L)), 4, screen->pcf8574_addr);
        _delay_us(100);

    i2cWritePol(\
        (char *)buildInstruction(HOME), 4, screen->pcf8574_addr);
        _delay_ms(2);
}


uint8_t newAddrLine4(SCREEN *screen){
    uint8_t current_column = screen->current_column;

    if(current_column < 19){
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


void setCursorDDRAM(uint8_t pos, uint8_t pcf_address){
    i2cWritePol((char *)buildInstruction(((1<<RAM_BIT) | pos)), 4, pcf_address); // Set DDRAM Address
    _delay_us(100);
}

void screenWriteDDRAM(SCREEN *screen, char *buffer){
    if(buffer == NULL) return;

    for(uint16_t i = 0; i < strlen(buffer); i++){
        uint8_t next_addr = (screen->conf.N) ? newAddrLine4(screen) : newAddrLine2(screen);
        setCursorDDRAM(next_addr, screen->pcf8574_addr); // Set DDRAM Address

        // This calculates the control byte *with* E=1
        uint8_t bytes[5] = {0};
        buildBytes(bytes, (uint8_t)buffer[i]);
        i2cWritePol((char *)bytes, 4, screen->pcf8574_addr);
        _delay_us(45);
    }
}

void setCursorCGRAM(uint8_t pos, uint8_t pcf_address){
    i2cWritePol((char *)buildInstruction((1 << (RAM_BIT - 1)) | pos), 4, pcf_address); // Set CGRAM address
    _delay_us(100);
}

void screenWriteCGRAM(SCREEN *screen, const uint8_t *pattern, uint8_t char_index) {
    if (!pattern) return;
    char_index &= 0x07;

    setCursorCGRAM(char_index * 8, screen->pcf8574_addr);

    for (uint8_t i = 0; i < 8; i++) {
        uint8_t bytes[5];
        buildBytes(bytes, pattern[i]);
        i2cWritePol((char *)bytes, 4, screen->pcf8574_addr);
        _delay_us(45);
    }
}

void screenClear(SCREEN *screen) {
    i2cWritePol((char *)buildInstruction(CLEAR_DISPLAY), 4, screen->pcf8574_addr);
    _delay_ms(2);
    screen->current_column = 0;
    screen->current_row = 0;
}

void screenSetCursor(SCREEN *screen, uint8_t row, uint8_t column) {
    screen->current_row = row;
    screen->current_column = column;
    uint8_t addr = getAddress(row, column);
    setCursorDDRAM(addr, screen->pcf8574_addr);
}

void screenWriteAt(SCREEN *screen, uint8_t row, uint8_t column, char *text) {
    screenSetCursor(screen, row, column);
    screenWriteDDRAM(screen, text);
}

void screenPrintCustomChar(SCREEN *screen, uint8_t row, uint8_t col, uint8_t char_index) { 
    screenSetCursor(screen, row, col); 
    uint8_t bytes[5]; buildBytes(bytes, char_index); 
    i2cWritePol((char *)bytes, 4, screen->pcf8574_addr); 
    _delay_us(45); 
}

void loadTileBatch(SCREEN *screen, LCDTile *tiles, uint8_t count) {
    for (uint8_t i = 0; i < count && i < 8; i++) {
        screenWriteCGRAM(screen, tiles[i].pattern, tiles[i].slot);
        screenPrintCustomChar(screen, tiles[i].row, tiles[i].col, tiles[i].slot);
    }
}

//SnakeGame
void snakeGameIcon(SCREEN *screen) {
    LCDTile snakeTiles[] = {
        {{0, 0, 6, 15, 15, 6, 0, 6},      0, 18, 0},
        {{0, 0, 0, 0, 0, 1, 1, 1},        1, 17, 1},
        {{15, 15, 6, 14, 28, 24, 16, 16}, 1, 18, 2},
        {{1, 0, 0, 0, 0, 0, 0, 0},        2, 17, 3},
        {{24, 24, 24, 28, 14, 7, 3, 1},   2, 18, 4},
        {{0, 0, 0, 0, 0, 0, 16, 16},      2, 19, 5},
        {{1, 1, 8, 16, 8, 4, 0, 0},       3, 18, 6},
        {{16, 16, 0, 0, 0, 0, 0, 0},      3, 19, 7},

    };
    loadTileBatch(screen, snakeTiles, sizeof(snakeTiles) / sizeof(LCDTile));
}

//Pacman
void pacManGameIcon(SCREEN *screen) {
    LCDTile pacmanTiles[] = {
        {{0, 0, 0, 1, 3, 3, 7, 7},         1, 17, 0},
        {{0, 0, 14, 31, 31, 27, 31, 30},   1, 18, 1},
        {{0, 0, 0, 16, 24, 16, 0, 4},      1, 19, 2},
        {{7, 3, 3, 1, 0, 0, 0, 0},         2, 17, 3},
        {{31, 31, 31, 31, 14, 0, 0, 0},    2, 18, 4},
        {{0, 16, 24, 16, 0, 0, 0, 0},      2, 19, 5},
    };
    loadTileBatch(screen, pacmanTiles, sizeof(pacmanTiles) / sizeof(LCDTile));
}


//Pong
void pongIcon(SCREEN *screen) {
    LCDTile pongTiles[] = {
        {{0, 0, 7, 7, 7, 7, 7, 7},         0, 17, 0},
        {{7, 7, 7, 7, 7, 7, 7, 7},         1, 17, 1},
        {{0, 0, 0, 12, 30, 30, 12, 0},     1, 19, 2},
        {{7, 7, 7, 7, 7, 7, 7, 7},         2, 17, 3},
        {{7, 7, 7, 7, 7, 7, 0, 0},         3, 17, 4},
    };
    loadTileBatch(screen, pongTiles, sizeof(pongTiles) / sizeof(LCDTile));
}								

//Spacecraft
void spacecraftIcon(SCREEN *screen) {
    LCDTile spacecraftTiles[] = {
        {{0, 0, 0, 0, 0, 0, 0, 8},         0, 18, 0},
        {{0, 4, 0, 2, 0, 4, 0, 8},         1, 18, 1},
        {{0, 0, 0, 0, 0, 0, 0, 3},         2, 17, 2},
        {{0, 4, 0, 0, 14, 14, 14, 17},     2, 18, 3},
        {{0, 0, 0, 0, 0, 0, 0, 24},        2, 19, 4},
        {{3, 3, 0, 0, 0, 0, 0, 0},         3, 17, 5},
        {{17, 17, 0, 0, 0, 0, 0, 0},       3, 18, 6},
        {{24, 24, 0, 0, 0, 0, 0, 0},       3, 19, 7},
    };
    loadTileBatch(screen, spacecraftTiles, sizeof(spacecraftTiles) / sizeof(LCDTile));
}


void updateGameIcon(SCREEN *screen, const char *game_name) {
    if (strcmp(game_name, "Snake") == 0) snakeGameIcon(screen);
    else if (strcmp(game_name, "Pac-Man") == 0) pacManGameIcon(screen);
    else if (strcmp(game_name, "Pong") == 0) pongIcon(screen);
    else if (strcmp(game_name, "Spacecraft") == 0) spacecraftIcon(screen);
}

void updateGameScreen(SCREEN *screen, char *game_name, uint16_t current_score, uint16_t top_score) {
    char top_buf[10];  
    char cur_buf[10];
    char title_buf[20];  

    snprintf(top_buf, sizeof(top_buf), "TS:%d", top_score);
    snprintf(cur_buf, sizeof(cur_buf), "CS:%d", current_score);
    snprintf(title_buf, sizeof(title_buf), "GAME: %s", game_name);

    screenWriteAt(screen, 0, 0, title_buf);  
    screenWriteAt(screen, 3, 0, cur_buf);     
    screenWriteAt(screen, 3, 9, top_buf);  
    updateGameIcon(screen, game_name);   
}													
