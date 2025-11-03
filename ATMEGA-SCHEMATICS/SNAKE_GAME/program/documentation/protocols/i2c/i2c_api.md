# I2C Firmware Configuration Reference

## Data Types

### `I2C_CONF`
This struct holds the configuration for the I2C protocol. A variable holding this strucut must be created before calling the I2C initialization. However, the user can either call init with a true argument for a default configuration or adjust it based on custom needs and initializing I2C with a false argument to bypass the default.

| Field | Type | Description |
|-------|------|-------------|
| `TWBR_VAL` | `uint32_t` | This value is calculated based on the other values set in the struct. This value is inserted in the I2C register and saved in this variable for debug purposes. |
| `frequency` | `uint32_t` | This is the maximum frequency you want to achieve. Note that this maximum speed has to stay within the limits of both the master and the slave. |
| `prescaler` | `uint8_t` | This is the parameter used in the formula given by the MCU datasheet to divide the speed set in frequency. Therefore, the protocol can be slowed down by given a higher prescaler. Note that the accepted values for prescaler are: 1, 4, 16 or 64. |
| `mode` | `uint8_t` | This sets the mode the MCU will work. The accepted opcodes are: MODE_MASTER_POL, MODE_MASTER_INT, MODE_SLAVE_POL, MODE_SLAVE_INT. Note That since the API is not yet complete, the only mode working at this point is: MODE_MASTER_POL. |
| `f_cpu` | `uint32_t` | This value has to be set to the current clock speed in the environment. Note that the arduino UNO is by default set to 16MHz or (16000000 Hz). |

Note: Notice as well that the I2C hardware configuration will get hold of both SDA and SCL pins. Therefore, they will not be able to be used as a GPIO, unless carefull configuration and unconfiguration routine is planned.

## Methods

### `void i2cInit(I2C_CONF *conf, bool default_conf)`

Initializes the I2C protocol with either a default configuration or a custom one.

- If `default_conf == true`, the I2C_CONF struct passed in the methods is filled with default values.
- If `default_conf == false`, you must manually set the values to configure the i2c protocol. Note that these values will be used in the formula:

    uint32_t twbr_num = (conf->f_cpu / conf->frequency);
    uint32_t twbr_den = 2*(conf->prescaler);
    uint8_t twbr = (uint8_t)((twbr_num - 16)/twbr_den);

    This calculation derives from the datasheet formula:
    frequency = (f_cpu)/(16 + 2*TWBR*prescaler)


### `uint8_t i2cWritePol(char *buffer, size_t size, uint8_t address)`

This method is used to write a string of a certain size to a target address. To understand how the address argument works, you should read a little about how the I2C packaging works. In this protocol, each slave is set with a predefined address, which is included in the first byte sent by the master when trying to communicate with devices connected in the data bus.


- When using this method, it will try to connect with the given address 10 times and skip the loop without sending the message if the target, .


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

