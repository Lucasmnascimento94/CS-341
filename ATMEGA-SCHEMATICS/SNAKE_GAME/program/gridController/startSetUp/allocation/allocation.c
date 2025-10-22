#include "allocation.h"


SnakeBelly belly = {
    .color = COLOR_GOLD,
    .color_food = COLOR_GREEN,
    .count = 0,
    .direction =LEFT,
    .head = NULL,
    .tail = NULL,
    .walk = false,
    .end = false,
    .begin = false,
    .pt = 0
};

struct Cell food = {
    .i = 8,
    .j = 5,
    .next = NULL,
    .poison = false,
    .prev = NULL,
    .val = PIXEL_ADDRESS(8, 5, 2) 
};

I2C_TARGET i2c_target = {
    .addr                = PCF8574_ADDR_WRITE,
    .direction           = I2C_WRITE,
    .frequency           = F_CPU/16,
    .mode                = MODE_MASTER_POL,
    .prescaler           = 1,
};

I2C_PORT i2c_port = {
    .data                = 0,
    .data_size           = 0,
    .instruction         = 0,
    .intruction_size     = 6,
    .current_target_addr = 0xFF,
    .current_mode        = 0xFF
    
};

SCREEN_CONF screen = {
    .pcf8574_addr = PCF8574_ADDR_WRITE
};

uint8_t allocateVar(){
    i2c_port.instruction = malloc(sizeof(char)*i2c_port.intruction_size);
    return (i2c_port.instruction != NULL) ? 1 : 0;
}