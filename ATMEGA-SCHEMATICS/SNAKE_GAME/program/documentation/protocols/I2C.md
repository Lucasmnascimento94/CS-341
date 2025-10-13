# SPI APPLICATION NOTE

**Author(s):** Lucas Nascimento 
**Date:** 10/01/2025
**Related Module:** protocols

# --------------------------------------------------------------------------------------------------------------- #
## Purpose

This application note explains how the **SPI protocol** is configured and used on the **ATmega168A**. 
It outlines how the **memory map** is used to set up the SPI hardware module, how to access the **SPI Data Register (SPDR)** to send and receive data, and how the related **control and status registers** are organized.  
The goal is to provide a clear reference so other teams can understand SPI operation without needing to parse the full datasheet.



# --------------------------------------------------------------------------------------------------------------- #
# Scope

- Serial Peripheral Interface protocol
- SPI Registers configuration



# --------------------------------------------------------------------------------------------------------------- #
# Configuration / Design

- **Hardware Settings**:
    GPIO #19 (PB5): **SCK**     > Clock 
    GPIO #18 (PB4): **MISO**    > Master-In-Slave-out
    GPIO #17 (PB3): **MOSI**    > Master-out-Slave-in
    GPIO #16 (PB2): **SS**      > Slave select

    **Clock** >> Defines the speed of the communication and is given by the MCU clock divided by the prescaler
    **MISO** >> Income data from the slave into the master
    **MOSI** >> Outgoing data from the master into the slave
    **SS** >> Chip Select. It is part of the protocol and initializes the communication


- **Wiring**

    MISO (master) <<<<<<>>>>>> MISO  (slave)
    MOSI (master) <<<<<<>>>>>> MOSI  (slave)
    SCK  (master) <<<<<<>>>>>> SCK   (slave)
    SS   (master) <<<<<<>>>>>> SS/CS (slave)
- **Modes of Operation**:

    - **Slave Mode**: In this mode, the Atmega will not be responsible for driving the clock and it will be reading data in the MOSI and writing data into MISO.
    When the SPI is configured as a Slave, the Slave Select (SS) pin is always input. When SS is held low, it works as a signal for the Atmega start the protocol, which is being initiated by the master.

    - **Master Mode**: In this mode, the Atmega will be responsible for driving the clock and it will be sending data in the MOSI and reading it in the MISO.



### Data Modes (CPOL/CPHA)

| SPI Mode | CPOL | CPHA | Leading Edge | Trailing Edge |
|----------|------|------|--------------|---------------|
| 0        | 0    | 0    | Rising (Sample) | Falling (Setup) |
| 1        | 0    | 1    | Rising (Setup)  | Falling (Sample) |
| 2        | 1    | 0    | Falling (Sample)| Rising (Setup)  |
| 3        | 1    | 1    | Falling (Setup) | Rising (Sample) |

# --------------------------------------------------------------------------------------------------------------- #
### Software SetUp **Registers**
    The registers responsible for this protocol are: TWRC, TWSR and TWDR

**TWCR** (I2C Control Register)
    This register is responsible to configure how the I2C will be operated. This register is formed by the following 8 bits:

      7   |   6   |   5   |    4   |   3   |   2   |  1   |  0   
    TWINT |  TWEA | TWSTA |  TWSTO |  TWWC |  TWEN |  -   | TWIE 


-  **TWINT**: TWI Interrupt Flag
    This bit is set by hardware when the TWI has finished its current job and expects application software response.
    If the I-bit in SREG and TWIE in TWCR are set, the MCU will jump to the TWI Interrupt Vector. While the TWINT
    Flag is set, the SCL low period is stretched. The TWINT Flag must be cleared by software by writing a logic one
    to it. Note that this flag is not automatically cleared by hardware when executing the interrupt routine. Also note
    that clearing this flag starts the operation of the TWI, so all accesses to the TWI Address Register (TWAR), TWI
    Status Register (TWSR), and TWI Data Register (TWDR) must be complete before clearing this flag

-   **TWEA**: TWI Enable Acknowledge Bit
    The TWEA bit controls the generation of the acknowledge pulse. If the TWEA bit is written to one, the ACK
    pulse is generated on the TWI bus if the following conditions are met:
    1. The device’s own slave address has been received.
    2. A general call has been received, while the TWGCE bit in the TWAR is set.    
    3. A data byte has been received in Master Receiver or Slave Receiver mode.
    By writing the TWEA bit to zero, the device can be virtually disconnected from the 2-wire Serial Bus temporarily.
    Address recognition can then be resumed by writing the TWEA bit to one again. 

-   **TWSTA**: TWI START Condition Bit
    The application writes the TWSTA bit to one when it desires to become a Master on the 2-wire Serial Bus. The
    TWI hardware checks if the bus is available, and generates a START condition on the bus if it is free. However,
    if the bus is not free, the TWI waits until a STOP condition is detected, and then generates a new START
    condition to claim the bus Master status. TWSTA must be cleared by software when the START condition has
    been transmitted.

-   **TWSTO**: TWI STOP Condition Bit
    Writing the TWSTO bit to one in Master mode will generate a STOP condition on the 2-wire Serial Bus. When
    the STOP condition is executed on the bus, the TWSTO bit is cleared automatically. In Slave mode, setting the
    TWSTO bit can be used to recover from an error condition. This will not generate a STOP condition, but the TWI
    returns to a well-defined unaddressed Slave mode and releases the SCL and SDA lines to a high impedance
    state.

-   **TWWC**: TWI Write Collision Flag
    The TWWC bit is set when attempting to write to the TWI Data Register – TWDR when TWINT is low. This flag
    is cleared by writing the TWDR Register when TWINT is high.

-   **TWEN**: TWI Enable Bit
    The TWEN bit enables TWI operation and activates the TWI interface. When TWEN is written to one, the TWI
    takes control over the I/O pins connected to the SCL and SDA pins, enabling the slew-rate limiters and spike
    filters. If this bit is written to zero, the TWI is switched off and all TWI transmissions are terminated, regardless of
    any ongoing operation.

-   **TWIE**: TWI Interrupt Enable
     When this bit is written to one, and the I-bit in SREG is set, the TWI interrupt request will be activated for as long
    as the TWINT Flag is high.



**TWSR** (I2C Status Register)
    While the purpose of the control register is to set configuration on the SPI protocol, the status register is used by
    the hardware to set flags, reflecting the current state machine of the protocol. These flags are used to check whether
    the data was sent, has arrived or if there was any interrupt caused by this specific protocol port.


      7   |   6   |   5   |   4   |   3    |   2  |     1   |   0   
    TWS7  |  TWS6 |  TWS5 |  TWS4 |  TWS3  |   -  |   TWPS1 | TWPS0

-   **TWS[7-3]**: TWI Status
    These 5 bits reflect the status of the TWI logic and the 2-wire Serial Bus. The different status codes are
    described later in this section. Note that the value read from TWSR contains both the 5-bit status value and the
    2-bit prescaler value. The application designer should mask the prescaler bits to zero when checking the Status
    bits. This makes status checking independent of prescaler setting. This approach is used in this datasheet,
    unless otherwise noted.

-   **TWPS[1-0]** TWI Prescaler Bits
    These bits can be read and written, and control the bit rate prescaler

    |   TWPS1  |  TWPS0 | Prescaler Value|
    |----------|--------|----------------|
    | 0        | 0      | 1              |
    | 0        | 1      | 4              |
    | 1        | 0      | 16             |
    | 1        | 1      | 64             |


**TWAR** (TWI (Slave) Address Register)

    The TWAR should be loaded with the 7-bit Slave address (in the seven most significant bits of TWAR) to which
    the TWI will respond when programmed as a Slave Transmitter or Receiver, and not needed in the Master
    modes. In multi master systems, TWAR must be set in masters which can be addressed as Slaves by other
    Masters.

      7   |   6   |   5   |   4   |   3    |   2   |    1   |   0   
    TWA6  |  TWA5 |  TWA4 |  TWA3 |  TWA2  |  TWA1 |   TWA0 | TWGCE

- **TWA[7-1]**: TWA: TWI (Slave) Address Register
    These seven bits constitute the slave address of the TWI unit.

- **TWGCE**: TWI General Call Recognition Enable Bit
    If set, this bit enables the recognition of a General Call given over the 2-wire Serial Bus.


**TWAMR** TWI (Slave) Address Mask Register

    The TWAR should be loaded with the 7-bit Slave address (in the seven most significant bits of TWAR) to which
    the TWI will respond when programmed as a Slave Transmitter or Receiver, and not needed in the Master
    modes. In multi master systems, TWAR must be set in masters which can be addressed as Slaves by other
    Masters.

      7   |    6   |    5   |    4   |    3    |    2   |    1   |   0   
    TWAM6 |  TWAM5 |  TWAM4 |  TWAM3 |  TWAM2  |  TWAM1 |  TWAM0 |   -

- **TWAM[7-1]**: TWA: TWI (Slave) Address Register
    These seven bits constitute the slave address of the TWI unit.


**TWDR**
    This is the data register for this specific protocol. After the SPI protocol has been configured, the next action to start a 
    transfer is to move data into this register, and the protocol will be handled by the hardware, shitfing the buffer bit by bit
    in either MSB or LSB, based on the control register configuration.



# --------------------------------------------------------------------------------------------------------------- #
## Implementation Flow


### MASTER MODE

Step-by-step description of how this feature is implemented:  
1. Initialization  
    - Configure Control Register **Crossing data with the target protocol expectations**
    - Clear flags in the Status Register **avoiding false triggers**
    - Enable protocol in Control Register
    
2. Data flow / command handling
    - Initilize communication by driving SS/CS Low
    - Send Data to register if you are sending data
    - Send Dummy byte if you are expecting to receive data (master mode)
    - Check SPIF in a loop to confirm that data the transmission is complete
    - Clear flag by reading it in the status register (done in the bullet above)
    - Drive SS/CS HIGH to end the communication

3. Error handling  
    - Check WCOL in the status register to confirm that there was no collision
    - CRC?


# --------------------------------------------------------------------------------------------------------------- #
## Testing & Validation

- logic analyzer.

- Unit test present at ./Documentation/unitTest

- Known limitations
    Current design uses a 16MHz external clock.

    -> Maximum speed = fosc/2 -> 8MHz
    -> Minimum speed = fosc 128 -> 125kHz


# --------------------------------------------------------------------------------------------------------------- #
## References
- https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf



# --------------------------------------------------------------------------------------------------------------- #
## Revision History
| Date       | Version | Author     | Notes/Changes |
|------------|---------|------------|---------------|
| MM/DD/YYYY | v1      | Lucas Nascimento     | Original |

