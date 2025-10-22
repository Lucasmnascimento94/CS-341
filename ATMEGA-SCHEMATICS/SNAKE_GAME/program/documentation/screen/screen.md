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
    The LCD screen is driven by a **PCF8574 GPIO expander**, which communicates with the microcontroller via the **I²C protocol**.  
    Because the PCF8574 provides only 8 GPIO pins, the LCD operates in **4-bit mode**.

    | PCF8574 Pin | LCD Pin | Description |
    |--------------|----------|-------------|
    | P0 | RS | Register Select |
    | P1 | R/W | Read / Write control |
    | P2 | E | Enable signal |
    | P3 | B | Backlight control |
    | P4 | D4 | Data bit 4 |
    | P5 | D5 | Data bit 5 |
    | P6 | D6 | Data bit 6 |
    | P7 | D7 | Data bit 7 |

- Writing Wave Form: 
    Each byte is sent in **two 4-bit transfers** — first the high nibble, then the low nibble.  
    During each transfer, the **Enable (E)** line toggles high–low while **RS**, **R/W**, and **DATA[4–7]** remain stable.



| Phase | RS | R/W | E (Enable) | DATA Lines | Description |
|--------|----|-----|-------------|-------------|--------------|
| 1 | Set (0 = Command, 1 = Data) | 0 (Write) | Low | D7–D4 = High nibble | Prepare high nibble |
| 2 | Hold | Hold | **High → Low** | D7–D4 = High nibble | Latch high nibble on **falling edge of E** |
| 3 | Hold | Hold | Low | D7–D4 = Low nibble | Prepare low nibble |
| 4 | Hold | Hold | **High → Low** | D7–D4 = Low nibble | Latch low nibble on **falling edge of E** |

> ⚙️ **Summary:**  
> - `RS` determines whether you’re sending a **command** (`RS = 0`) or **data** (`RS = 1`).  
> - `R/W` remains **0** for writing.  
> - Each nibble is latched when **E transitions from HIGH to LOW**.  
> - Timing between nibbles must meet the LCD’s setup/hold requirements (typically a few microseconds).



## LCD Opcodes Reference

Below are the available LCD command opcodes and their functions.  
All timing values assume standard HD44780/PCF8574 operation at 5 V.

---

### **CLEAR_DISPLAY** — Clear Display *(~2 ms)*  
**Opcode:** `0x01`  **[RS = 0, R/W = 0]`

Clears all DDRAM data by writing spaces (`0x20`) to every address, then resets the DDRAM address counter (AC) to `0x00`.

**Effect:**
- Cursor returns to the top-left position (row 0, column 0).  
- Entry mode is set to increment (`I/D = 1`).

---

### **HOME** — Return Cursor Home *(~2 ms)*  
**Opcode:** `0x02`  **[RS = 0, R/W = 0]`

Moves the cursor to the original position (address 0).  
Display shift (if active) is cleared.  
DDR content is not modified.

---

### **ENTRY_MODE** — Entry Mode Set *(~39 µs)*  
**Opcode:** `0x04 | 0b0000 00(I/D)(SH)`  **[RS = 0, R/W = 0]**

Controls cursor direction and display shift behavior.

| Bit | Name | Description |
|------|------|-------------|
| I/D | Increment/Decrement | 1 = Cursor moves right (address ++); 0 = Cursor moves left (address – –) |
| SH | Shift Display | 1 = Shift entire display during write; 0 = No shift |

> 💡 *Both DDRAM and CGRAM follow the same increment/decrement rule.*

---

### **DISPLAY_ON_OFF** — Display Control *(~39 µs)*  
**Opcode:** `0x08 | 0b0000 0(D)(C)(B)`  **[RS = 0, R/W = 0]**

Controls display power, cursor visibility, and blinking.

| Bit | Name | Description |
|------|------|-------------|
| D | Display On | 1 = Display on 0 = Display off (memory retained) |
| C | Cursor On | 1 = Show cursor 0 = Hide cursor |
| B | Blink On | 1 = Blink cursor 0 = Static cursor |

---

### **CURSOR** — Cursor / Display Shift *(~39 µs)*  
**Opcode:** `0x10 | 0b0000 (S/C)(R/L)00`  **[RS = 0, R/W = 0]**

Moves cursor or shifts entire display without altering DDRAM data.

| S/C | R/L | Operation |
|------|------|------------|
| 0 | 0 | Move cursor left (AC – 1) |
| 0 | 1 | Move cursor right (AC + 1) |
| 1 | 0 | Shift entire display left |
| 1 | 1 | Shift entire display right |

> During 2-line mode, shifting affects all lines simultaneously.

---

### **FUNCTION_SET** — Interface Configuration *(~39 µs)*  
**Opcode:** `0x20 | 0b000(DL)(N)(F)00`  **[RS = 0, R/W = 0]**

Sets data length, number of display lines, and font.

| Bit | Name | Description |
|------|------|-------------|
| DL | Data Length | 1 = 8-bit mode 0 = 4-bit mode |
| N | Lines | 1 = 2-line display 0 = 1-line display |
| F | Font | 1 = 5×11 dots 0 = 5×8 dots |

---

### **SET_CGRAM_ADDR** — Set CGRAM Address *(~39 µs)*  
**Opcode:** `0x40 | 0b00(AC5)(AC4)(AC3)(AC2)(AC1)(AC0)`  **[RS = 0, R/W = 0]**

Sets the CGRAM address for custom character writes.

---

### **SET_DDRAM_ADDR** — Set DDRAM Address *(~39 µs)*  
**Opcode:** `0x80 | 0b0(AC6)(AC5)(AC4)(AC3)(AC2)(AC1)(AC0)`  **[RS = 0, R/W = 0]**

Sets the DDRAM address counter.  
- 1-line mode (N = 0): `00h–4Fh`  
- 2-line mode (N = 1): line 1 → `00h–27h`, line 2 → `40h–67h`

---

### **READ_BUSY_FLAG** — Read Busy Flag & Address *(~39 µs)*  
**Opcode:** `0b(BF)(AC6)(AC5)(AC4)(AC3)(AC2)(AC1)(AC0)`  **[RS = 0, R/W = 1]**

Reads the busy flag and current address counter.  
- `BF = 1`: internal operation in progress — wait until cleared.

---

### **WRITE_TO_RAM** — Write Data *(~43 µs)*  
**Opcode:** `0b(DB7)(DB6)(DB5)(DB4)(DB3)(DB2)(DB1)(DB0)`  **[RS = 1, R/W = 0]**

Writes 8-bit data to DDRAM or CGRAM (depending on previous address command).  
Address automatically increments or decrements per **ENTRY_MODE**.

---

### **READ_FROM_RAM** — Read Data *(~43 µs)*  
**Opcode:** `0b(DB7)(DB6)(DB5)(DB4)(DB3)(DB2)(DB1)(DB0)`  **[RS = 1, R/W = 1]**

Reads 8-bit data from DDRAM or CGRAM.  
The first read after an address change is invalid (dummy read).  
Subsequent reads are valid.  
Address counter auto-increments/decrements after each read.

## Implementation Flow

This section describes how the LCD driver is organized and how control/data moves through the system.

### 1) Initialization

1. **Create globals**
   - `I2C_CONF i2c;`
   - `SCREEN screen;`

2. **Initialize I²C**
   - `i2cInit(&i2c, /*default_conf=*/true);`
   - Programs TWBR/TWPS based on `f_cpu`, target SCL, and prescaler.

3. **Initialize LCD**
   - `screenInit(&screen);`
   - Loads defaults (`screenDefault`), sends:
     - `FUNCTION_SET`
     - `CLEAR_DISPLAY` (≥1.52 ms)
     - `ENTRY_MODE` (I/D, SH)
     - `DISPLAY_ON_OFF` (D/C/B)
     - `HOME`
   - Sets software cursor: `current_row = 0`, `current_column = 0`.

---

### 2) Data flow / command handling

**High-level API:**
- `screenWrite(&screen, const char *buf)`
  - For each character:
    1. Compute DDRAM address with `newAddrLine4()` (returns *current* address, then advances internal `(row,col)` for next char).
    2. `setCursor(addr, screen.pcf8574_addr)` → issues `SET_DDRAM_ADDR`.
    3. Pack the byte into two 4-bit writes via `buildDataNibbles()` (RS=1, R/W=0, E high→low for each nibble).
    4. `i2cWritePol(pkt, 4, screen.pcf8574_addr)` with inter-char delay (≥37 µs).

**Address helpers:**
- `getAddress(row, column)` → returns DDRAM address for `(row, col)`.
- `newAddrLine4()` → uses internal `current_row/current_column` to maintain the “next write” position and wrap across lines.

**Command packing:**
- `buildInstruction(opcode)` (RS=0 path) → emits 4 bytes (hi nibble with E, hi without E, lo with E, lo without E).
- `buildDataNibbles(byte)` (RS=1 path) → same pattern for data writes.

---

## References

- [Datasheets used](https://github.com/Lucasmnascimento94/CS-341/tree/SNAKE_V4_PROGRAMMER_LUCAS/ATMEGA-SCHEMATICS/SNAKE_GAME/datasheet/SCREEN)
- [Internal cross-references](https://github.com/Lucasmnascimento94/CS-341/tree/SNAKE_V4_PROGRAMMER_LUCAS/ATMEGA-SCHEMATICS/SNAKE_GAME/program/documentation/protocols/i2c)

---

## Revision History

| Date       | Version | Author     | Notes/Changes |
|------------|---------|------------|---------------|
| MM/DD/YYYY | v1.1    | [Lucas_Nascimento]     | Initial draft |












0b0010 1000

1 - 0010 1100
2 - 1000 1100