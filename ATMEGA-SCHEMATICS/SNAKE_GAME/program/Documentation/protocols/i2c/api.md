# Data Types

**I2C_CONF**
- uint32_t TWBR_VAL
    - Holds the value stores in the TWBR register that is used for the clock generator

- uint8_t prescaler (DEFAULT)
    - Sets the value which will be used to fraction the clock used in the protocol

- bool read_mode
    - True if it is reading, false if it is writing

- bool master_mode
    - True if it is the master, false if it is the slave

- bool interrupt_mode
    - True if it is in interrupt Mode, false if it is in Polling mode

**I2C_SLAVE / I2C_MASTER**
- char *data
- char *instruction
- uint8_t prescaler
- uint8_t addr
- 
    char *data;
    char *instruction;
    uint16_t twbr;
    uint8_t prescaler;
    bool read_mode;
    bool master_mode;
    bool interrupt_mode;
    bool polling_mode;
    I2C_MASTER **master;
    I2C_SLAVE **slave;
    uint32_t frequency;
    uint32_t TWBR_VAL;
    uint8_t addr;
    uint8_t count;



