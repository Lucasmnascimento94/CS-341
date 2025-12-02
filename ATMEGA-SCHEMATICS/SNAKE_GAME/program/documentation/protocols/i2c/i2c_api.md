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


- When using this method, it will try to connect with the given address 10 times and skip the loop without sending the message if the target isn't found.

### `uint8_t i2cWritePol_(char *buffer, size_t size)`

This method can be used to achieve the same result and <uint8_t i2cWritePol()>. This function however, does not call a private method <i2cPolHelper()> to start the protocol. Therefore, by using this method you have to call <i2cStartPol()> first, which will use the address argument and mode to connect to the target. If This method is used without the i2cStart, it will send the message using the protocol, but since the start signal was not initiated no target will ever respond to it. However, you can also use this method to check that the data you are sending is the one you are expecting by using an analyzer.

### `uint8_t i2cStartPol(uint8_t address, uint8_t mode)`

This method is a helper, which has to be used to send a start signal to the data bus, if the users wants to control when the protocol has to start and stop.

### `uint8_t i2cStop()`

This method is a helper, which has to be used to send a stop signal to the data bus, if the user wants to control when the protocol has to start and stop.



### Using the API:
1. Initialization  
    - Create a variable of the type I2C_CONF. You can either configure the parameters or just leave it blank.
    `i2cInit(I2C_CONF *conf, bool default_conf)` :: Call this function, sending the pointer from the I2C_CONF variable, and a true or false for default configuration. 
    - default_conf = TRUE - If you are not configuring the paramaters and wants to use the default values.
    - default_conf = FALSE - If you are adjusting the parameters and wants to bypass the default values.  

2. Writing
    - `i2cWritePol(char *buffer, size_t size, uint8_t address)`:: Call this method you have a target connected and doesn't need to control the start/stop or does not need to keep the slave and master connected once the buffer is shifted out.

    - `i2cWritePol_(char *buffer, size_t size)` :: Call this method if you either need to control start/stop in a different way or if you just want to send data in the data bus to check output in an analyzer.
    

### NOTE:

Since the API is not yet complete, there are the only method supported at this point.
