# [TOPIC] APPLICATION NOTE

**Author(s):** Lucas Nascimento
**Date:** 10/13/2025 
**Related Module:** GAME-SCREEN

---

## Purpose

This document explains how **SCREEN** is handled in this project.  
It is intended to provide a clear reference for other teams so they can understand the design decisions, configuration, and testing methods without having to parse datasheets directly.

---

## Scope

- Define what this note covers and what it does not.  
- Mention which part of the system depends on it (e.g., external memory, programmer, LED grid).  

---

## Configuration / Design

- Pinout
    The LCD screen has the following pinout:
    **VSS**     : GROUND
    **VDD**     : 5V
    **VO**      : Constrast adjust
    **RS**      : Register Select (It controls whether the byte is data or command)
    **R/W**     : Signal if you are in reading or writing mode
    **E**       : Operation (data read/write) enable signal
    **DB0-DB3** : Can be ignored in 4-bit mode operation
    **DB4-DB7** : Used for data transfer (in this set up, only the high nibble will be used)
    **A(LED+)** : LED ANODE
    **K(LED-)** : LED KATHOD

- Hardware settings.
    The LCD screen is controlled by a GPIO expander (PCF8574), which is controlled through I2C protocol.
    Since the PCF8574 only has 8 pins, the screen will be used in 4-bit mode operation with the following wiring:

    P0 (PCF8574) <<<<<<>>>>>> RS  (SCREEN)
    P1 (PCF8574) <<<<<<>>>>>> R/W (SCREEN)
    P2 (PCF8574) <<<<<<>>>>>> E   (SCREEN)
    P3 (PCF8574) <<<<<<>>>>>> B   (SCREEN)
    P4 (PCF8574) <<<<<<>>>>>> D4  (SCREEN)
    P5 (PCF8574) <<<<<<>>>>>> D5  (SCREEN)
    P6 (PCF8574) <<<<<<>>>>>> D6  (SCREEN)
    P7 (PCF8574) <<<<<<>>>>>> D7  (SCREEN)

- Writing Wave Form:
    RS   [0-1]
    R/W  [0]
    E    [1]
    DATA [D4-D7]
    E    [0]
    ...

- Writing Wave Form:
    RS   [0-1]
    R/W  [1]
    E    [1]
    DATA [D4-D7]
    E    [0]
    ...


- Opcodes:
    **CLEAR_DISPLAY** : Clear Display (2ms)
    [RS::0] [R/W::0] :: 0X01

        Clear all the display data by writing “20H” (space code) to all DDRAM address, and set DDRAM address to “00H” into AC (address counter).
        Return cursor to the original status, namely, bring the cursor to the left edge on the fist line of the display.
        Make the entry mode increment (I/D=“High”).
    
    **HOME** : Return Home (2ms)
    [RS::0] [R/W::0] :: 0X02

        Return home is cursor return home instruction.
        Set DDRAM address to “00H” into the address counter.
        Return cursor to its original site and return display to its original status, if shifted.
        Contents of DDRAM does not change.

    **ENTRY_MODE** : Entry Mode (39us)
    [RS::0] [R/W::0] :: (0X04) | 0b0000 00(I/D)(SH)

        Set the moving direction of cursor and display.
        
        I/D: increment / decrement of DDRAM address (cursor or blink)
            When I/D=“high”, cursor/blink moves to right and DDRAM address is increased by 1.
            When I/D=“Low”, cursor/blink moves to left and DDRAM address is increased by 1.   
            *CGRAM operates the same way as DDRAM, when reading from or writing to CGRAM.
        
        SH: shift of entire display
            When DDRAM read (CGRAM read/write) operation or SH=“Low”, shifting of entire display is not performed. If
            SH =“High” and DDRAM write operation, shift of entire display is performed according to I/D value. (I/D=“high”.
            shift left, I/D=“Low”. Shift right).

    **DISPLAY_ON_OFF**  : Display ON/OFF Control
    [RS::0] [R/W::0] :: (0X08) | 0b0000 0(D)(C)(B)

        D: Display ON/OFF control bit
            When D=“High”, entire display is turned on.
            When D=“Low”, display is turned off, but display data remains in DDRAM.

        C: cursor ON/OFF control bit
            When D=“High”, cursor is turned on.
            When D=“Low”, cursor is disappeared in current display, but I/D register preserves its data.

        B: Cursor blink ON/OFF control bit
            When B=“High”, cursor blink is on, which performs alternately between all the “High” data and display
            characters at the cursor position.
            When B=“Low”, blink is off.

    **CURSOR** : Cursor or Display Shift (39us)
    [RS::0] [R/W::0] :: (0X10) | | 0b0000 (S/C)(R/L)00

        Shifting of right/left cursor position or display without writing or reading of display data.
        This instruction is used to correct or search display data.
        During 2-line mode display, cursor moves to the 2nd line after the 40th digit of the 1st line.
        Note that display shift is performed simultaneously in all the lines.
        When display data is shifted repeatedly, each line is shifted individually.
        When display shift is performed, the contents of the address counter are not changed.

        |   S/C |   R/L |                                   Operation                                   |
        |   0   |   0   |   Shift cursor to the left, AC is decreased by 1                              |
        |   0   |   1   |   Shift cursor to the right, AC is increased by 1                             |
        |   1   |   0   |   Shift all the display to the left, cursor moves according to the display    |
        |   1   |   1   |   Shift all the display to the right, cursor moves according to the display   |

    **FUNCTION_SET** : Function Set   (39us)
    [RS::0] [R/W::0] :: (0x20) | 0b000(DL) (N)(F)00

        DL: Interface data length control bit
            When DL=“High”, it means 8-bit bus mode with MPU.
            When DL=“Low”, it means 4-bit bus mode with MPU. Hence, DL is a signal to select 8-bit or 4-bit bus mode.
            When 4-but bus mode, it needs to transfer 4-bit data twice.
        
        N: Display line number control bit
            When N=“Low”, 1-line display mode is set.
            When N=“High”, 2-line display mode is set.
        
        F: Display line number control bit
        When F=“Low”, 5x8 dots format display mode is set.
        When F=“High”, 5x11 dots format display mode.


    **SET_CGRAM_ADDR** : Set CGRAM ADDRESS (39us)
    [RS::0] [R/W::0]  0X40 | 0b00(AC5)(AC4) (AC3)(AC2)(AC1)(AC0)

        Set CGRAM address to AC.
        The instruction makes CGRAM data available from MPU.

    **SET_DDRAM_ADDR** : Set DDRAM Address (39us)
    [RS::0] [R/W::0] :: 0X80 | 0b0(AC6)(AC5)(AC4) (AC3)(AC2)(AC1)(AC0)

        Set DDRAM address to AC.
        This instruction makes DDRAM data available form MPU.
        When 1-line display mode (N=LOW), DDRAM address is form “00H” to “4FH”.In 2-line display mode (N=High),
        DDRAM address in the 1st line form “00H” to “27H”, and DDRAM address in the 2nd line is from “40H” to
        “67H”.

    **READ_BUSY_FLAG** : 
    [RS::0] [R/W:1] :: 0b(BF)(AC6)(AC5)(AC4) (AC3)(AC2)(AC1)(AC0)

        This instruction shows whether S6A0069 is in internal operation or not.
        If the resultant BF is “High”, internal operation is in progress and should wait BF is to be LOW, which by then
        the nest instruction can be performed. In this instruction you can also read the value of the address counter.

    **WRITE_TO_RAM** : Write data to Address (43us)
    [RS::1] [R/W::0] :: 0b(DB7)(DB6)(DB5)(DB4) (DB3)(DB2)(DB1)(DB0)

        Write binary 8-bit data to DDRAM/CGRAM.
        The selection of RAM from DDRAM, and CGRAM, is set by the previous address set instruction (DDRAM
        address set, CGRAM address set).
        RAM set instruction can also determine the AC direction to RAM.
        After write operation. The address is automatically increased/decreased by 1, according to the entry mode.

    **READ_FROM_RAM**  : Read data From RAM (43us)
    [RS::1] [R/W::1] :: 0b(DB7)(DB6)(DB5)(DB4) (DB3)(DB2)(DB1)(DB0)

        Read binary 8-bit data from DDRAM/CGRAM.
        The selection of RAM is set by the previous address set instruction. If the address set instruction of RAM
        is not performed before this instruction, the data that has been read first is invalid, as the direction of AC is not
        yet determined. If RAM data is read several times without RAM address instructions set before, read operation,
        the correct RAM data can be obtained from the second. But the first data would be incorrect, as there is no
        time margin to transfer RAM data.
        In case of DDRAM read operation, cursor shift instruction plays the same role as DDRAM address set
        instruction, it also transfers RAM data to output data register.
        After read operation, address counter is automatically increased/decreased by 1 according to the entry
        mode.
        After CGRAM read operation, display shift may not be executed correctly.
        NOTE: In case of RAM write operation, AC is increased/decreased by 1 as in read operation.
        At this time, AC indicates next address position, but only the previous data can be read by the read
        instruction.

---

## Implementation Flow

Step-by-step description of how this feature is implemented:  
1. Initialization  
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
| MM/DD/YYYY | v0.1    | [Name]     | Initial draft |
| MM/DD/YYYY | v0.2    | [Name]     | Updates/fixes |