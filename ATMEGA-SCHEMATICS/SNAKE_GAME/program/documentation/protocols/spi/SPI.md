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
    The registers responsible for this protocol are: SPCR, SPSR and SPDR

**SPCR** (SPI Control Register)
    This register is responsible to configure how the SPI will be operated. This register is formed by the following 8 bits:

      7   |   6  |  5   |   4   |   3   |   2   |  1   |  0   
    SPIE  |  SPE | DORD |  MSTR |  CPOL |  CPHA | SPR1 | SPR0 


-  **SPIE**: 
    This bit causes the SPI interrupt to be executed if SPIF bit in the SPSR Register is set and the if the Global
    Interrupt Enable bit in SREG is set.
-   **SPE**:
    When the SPE bit is written to one, the SPI is enabled. This bit must be set to enable any SPI operations.
-   **DORD**:
    When the DORD bit is written to one, the LSB of the data word is transmitted first.
    When the DORD bit is written to zero, the MSB of the data word is transmitted first.
-   **MSTR**:
    This bit selects Master SPI mode when written to one, and Slave SPI mode when written logic zero. If SS is
    configured as an input and is driven low while MSTR is set, MSTR will be cleared, and SPIF in SPSR will
    become set. The user will then have to set MSTR to re-enable SPI Master mode.
-   **CPOL**:
    When this bit is written to one, SCK is high when idle. When CPOL is written to zero, SCK is low when idle.
    Refer to Figure 19-3 and Figure 19-4 for an example. The CPOL functionality is summarized below
-   **CPHA**:
    The settings of the Clock Phase bit (CPHA) determine if data is sampled on the leading (first) or trailing (last)
    edge of SCK. 
-   **SPR1** & **SPR0**:
     These bits control the SCK rate. Note: In our project, the bit-level configuration logic for the prescaler is 
     abstracted into the **start.h** file, being necesary onlt to choose one of the value option found in the same file.

**SPSR** (SPI Status Register)
    While the purpose of the control register is to set configuration on the SPI protocol, the status register is used by
    the hardware to set flags, reflecting the current state machine of the protocol. These flags are used to check whether
    the data was sent, has arrived or if there was any interrupt caused by this specific protocol port.


      7   |   6   |  5   |   4   |   3   |   2   |  1   |  0   
    SPIF  |  WCOL |   -  |    -  |    -  |    -  |   -  | SPI2X

-   **SPIF**
    When a serial transfer is complete, the SPIF Flag is set. An interrupt is generated if SPIE in SPCR is set and
    global interrupts are enabled. If SS is an input and is driven low when the SPI is in Master mode, this will also
    set the SPIF Flag. SPIF is cleared by hardware when executing the corresponding interrupt handling vector.
    Alternatively, the SPIF bit is cleared by first reading the SPI Status Register with SPIF set, then accessing the
    SPI Data Register (SPDR).
-   **WCOL**
    The WCOL bit is set if the SPI Data Register (SPDR) is written during a data transfer. The WCOL bit (and the
    SPIF bit) are cleared by first reading the SPI Status Register with WCOL set, and then accessing the SPI Data
    Register.
-   **[5-1]**
    Reserved
-   **SPI2X** 
    When this bit is written logic one the SPI speed (SCK Frequency) will be doubled when the SPI is in Master
    mode (see Table 19-5). This means that the minimum SCK period will be two CPU clock periods. When the SPI
    is configured as Slave, the SPI is only ensured to work at fosc/4 or lower.
    The SPI interface on the ATmega48A/PA/88A/PA/168A/PA/328/P is also used for program memory and
    EEPROM downloading or uploading. See page 303 for serial programming and verification.

**SPDR**
    This is the data register for this specific protocol. After the SPI protocol has been configured, the next action to start a 
    transfer is to move data into this register, and the protocol will be handled by the hardware, shitfing the buffer bit by bit
    in either MSB or LSB, based on the control register configuration.



# --------------------------------------------------------------------------------------------------------------- #
## Implementation Flow

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

