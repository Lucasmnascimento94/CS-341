# SPI APPLICATION NOTE

**Author(s):** Lucas Nascimento 
**Date:** 10/01/2025
**Related Module:** protocols


## Purpose

This application note explains how the **SPI protocol** is configured and used on the **ATmega168A**. 
It outlines how the **memory map** is used to set up the SPI hardware module, how to access the **SPI Data Register (SPDR)** to send and receive data, and how the related **control and status registers** are organized.  
The goal is to provide a clear reference so other teams can understand SPI operation without needing to parse the full datasheet.


# Scope

- Serial Peripheral Interface protocol
- SPI Registers configuration

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


### Software SetUp **Registers**
    The registers responsible for this protocol are: SPCR, SPSR and SPDR

**SPCR** (SPI Control Register)
    This register is responsible to configure how the SPI will be operated. This register is formed by the following 8 bits:

      7   |   6  |  5   |   4   |   3   |   2   |  1   |  0   
    SPIE  |  SPE | DORD |  MSTR |  CPOL |  CPHA | SPR1 | SPR0 


-   SPIE: 