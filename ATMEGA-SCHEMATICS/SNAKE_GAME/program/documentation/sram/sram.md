# SRAM APPLICATION NOTE

**Author(s):** Lucas Nascimento 
**Date:** 10/08/2025  
**Related Module:** SRAM

---

## Purpose

This document explains how the external SRAM is handled in this project, how it is configured and how you can use the current API to access it.

---

## Scope

- Read data from SRAM
- Write data to the SRAM
- Read Mode Register
- Write Mode Register

---

## Configuration / Design

- The SRAM interface utilizes SPI protocol to exchange data, which is an available feature in our atmega168 MCU. Therefore, the SRAM-MCU communication is built on top
  of the SPI configured prior.

- Hardware Configuration:

    **Wiring**
    MCU_MOSI <<<<<<>>>>>> SRAM_SI/SIOO
    MCU_MISO <<<<<<>>>>>> SRAM_SO/SIO1
    MCU_SCK  <<<<<<>>>>>> SRAM_SCK
    MCU_PB1  <<<<<<>>>>>> SRAM_CS
    (TO DO)  <<<<<<>>>>>> SRAM_HOLD

    **SPI Requirement**
    | SPI Mode | CPOL | CPHA | Leading Edge | Trailing Edge |
    |----------|------|------|--------------|---------------|
    | 0        | 0    | 0    | Rising (Sample) | Falling (Setup) |

    Note: This is mode 0 in this project spi.

    **Instructions Structure**
    - Writing:
        send a 32bits instruction as:
        [31-23]               [23-0]
        [Write_Instruction]   [Memory_Address]

        Send Data
        [Send_String]

    - Reading:
        send a 32bits instruction as:
        [31-23]              [23-0]
        [Read_Instruction]   [Memory_Address]
        
        Read Data
        [Read_String]

    - Writing Mode Resgister:
        send a 16bits instruction as:
        [15-8]                              [7-0]
        [Write_Mode_Register_Instruction]   [Mode_Register_Value]

    - Reading Mode Resgister:
        send a 8bits instruction as:
        [15-8]                              
        [Read_Mode_Register_Instruction]    
        
        Read Mode register Value
        [Mode_Register_Value]  
---

## Implementation Flow

Step-by-step description of how this feature is implemented:  
1. Initialization  
    - 
2. Data flow / command handling  
3. Error handling  
4. Interaction with other modules  

---

## Testing & Validation

- Recommended test methods (logic analyzer, oscilloscope, UART logs, etc.).  
- Expected outcomes
- Known limitations

---

## References

- Datasheets used  
- External application notes or guides  
- Internal cross-references (e.g., link to related AN docs in this repo)

---

## Revision History

| Date       | Version | Author     | Notes/Changes |
|------------|---------|------------|---------------|
| 10/08/2025 | v0.1    | [Lucas Nascimento]     | Initial draft |