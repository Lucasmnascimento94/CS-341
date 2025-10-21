##### DATA TYPES ######

**I2C_PORT**
    This struct holds the data from where the reading/writing function will read/write data from/into

- char *data
    - Store data buffer to be sent out or to store data coming in.

- char *instruction
    - Store temporary command instructions

- uint8_t last_target
    - Holds the address of the last target address


**I2C_TARGET**
    This struct holds the target info.

- uint32_t TWBR_VAL
    - Holds the value stores in the TWBR register that is used for the clock generator

- uint8_t prescaler
    - Sets the value which will be used to fraction the clock used in the protocol

- uint8_t addr
    - Holds the target address
    
- uint8_t direction
    - Can be either **read** (1) or **write** (0)

- uint8_t mode (HIGH NIBBLE:: MASTER(0)/SLAVE(1) | LOW NIBBLE:: POLLING(0)/INTERRUPT(1))
    - Can be either 
    **master + polling mode** (0X00)
    **master + interrupt mode** (0X01)
    **slave + polling mode** (0X10)
    **slave + interrupt mode** (0X11)



# I2C Configuration
-
    **void i2cClockConfig(I2C_TARGET *target)**
        This function sets the clock (frequency) based on the target maximum speed


# I2C Start/Stop Function
-
    **uint8_t start_POL(I2C_TARGET *target)**
        This function starts the protocol in two steps:

        - Start:
            It sends a start signal on the SDA bus
        - Sends target address on SDA bus
        - Return status flasg
    
-
    **uint8_t start_INT(I2C_TARGET *target)**
        This function starts the protocol, setting up the registers to work in interrupt mode.

-
    **uint8_t i2cSTOP()**
        This function is a helper for a caller that is handling start/stop externally
        

# I2C Writing Functions
-
    **uint8_t i2cMasterWrite_POL(I2C_PORT *port, I2C_TARGET *target)**
        This function writes a data stream stored in pot to the target.
-
    **uint8_t i2cMasterWriteNoCtrl_POL(I2C_PORT *port, I2C_TARGET *target)**
        This function write a data stream stored in port to the target. However, instead
        of handling the Start/Stop signal in the SDA bus, this task is delagated to the caller.
        Therefore, Call a starter function before using this function and call a stopper afterwards.
    

##### METHODS ######


**_uint8_t i2cStartPOL(uint8_t address, uint8_t Mode)_** 

-   Starts I2C protocol in Polling mode. To initialize the protocol, the first instrucion has to be the target address + W/R command, 
    usually refered to SLA+W.

    <uint8_t address>   Target Address
    <uint8_t Mode>      O to write, 1 to read.

**_uint8_t i2cStartInt(uint8_t address, uint8_t Mode)_** 

-   Starts I2C protocol in Interrupt mode. To initialize the protocol, the first instrucion has to be the target address + W/R command, 
    usually refered to SLA+W.

    <uint8_t address>   Target Address
    <uint8_t Mode>      O to write, 1 to read.


**_uint8_t i2cSTOP()_**

-   Sends a stop signal to the I2C bus line. It is necessary to ensure that a Stop signal is sent to the bus line, otherwise the target
    will never free the bus.



**_uint8_t i2cWritePOL(char *buffer, size_t size, uint8_t address)_**

-   Sends a data buffer in polling mode to the a target address;

    <char *buffer>    Data being sent
    <size_t size>     Length of bytes being sent
    <uint8_t address> Target address
    <return:: (WSR & I2C_TWSR_FLAG_MASK)  > Returns the flag to indicate the status of the communication


**_uint8_t i2cWritePOL_(char *buffer, size_t size)_**

-   Send a data buffer in polling modewithout managing the I2C start signal. For some applications, the start/stop signal has to be 
    handled more loosely. Therefore, you can use this method combined with i2cStart() and i2cStop() to control the flow externally

    <char *buffer>    Data being sent
    <size_t size>     Length of bytes being sent