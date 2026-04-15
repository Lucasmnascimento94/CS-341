#include "allocation.h"



I2C_TARGET i2c_target = {
    .addr                = PCF8574_ADDR_WRITE,
    .direction           = I2C_WRITE,
    .frequency           = F_CPU/16,
    .mode                = MODE_MASTER_POL,
    .prescaler           = 1,
};

I2C_PORT i2c_port = {
    .data                = NULL,
    .data_size           = 0,
    .instruction         = NULL,
    .intruction_size     = 6,
    .current_target_addr = 0xFF,
    .current_mode        = 0xFF
};

uint8_t allocateVar(){
    i2c_port.instruction = malloc(sizeof(char)*i2c_port.intruction_size);
    return 0;
}