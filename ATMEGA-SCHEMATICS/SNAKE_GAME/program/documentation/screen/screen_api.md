# Screen Driver Reference

## Data Types

### `SCREEN`

Holds the runtime state of a display device connected through a PCF8574 I/O expander.  
Tracks the current cursor position and configuration used during I²C communication.

| Field | Type | Description |
|-------|------|-------------|
| `pcf8574_addr` | `uint8_t` | 7-bit I²C address of the screen’s PCF8574 expander. |
| `current_row` | `uint8_t` | Current row index where the next character will be written. |
| `current_column` | `uint8_t` | Current column index where the next character will be written. |
| `conf` | `SCREEN_CONF` | Configuration struct that defines display mode, cursor, and format settings. |

---

### `SCREEN_CONF`

Defines the display configuration bits for entry mode, cursor, display, and function settings.  
These fields map directly to the HD44780 instruction set.

| Fields | Type | Description |
|--------|------|-------------|
| `I_D`, `SH` | `uint8_t` | Entry mode bits — increment/decrement and shift configuration. |
| `D`, `C`, `B` | `uint8_t` | Display control bits — display on/off, cursor visibility, and blink. |
| `S_C`, `R_L` | `uint8_t` | Cursor/Display shift control bits. |
| `DL`, `N`, `F` | `uint8_t` | Function set bits — data length, number of display lines, and font type. |

---

## Methods

### `void screenInit(SCREEN *screen, bool default_conf)`

Initializes the display with either a default configuration or a custom one.

- If `default_conf == true`, the display is configured with default values (increment mode, display on, 2/4 lines).
- If `default_conf == false`, you must manually set `screen->conf` before calling this function.

---

### `void screenWrite(SCREEN *screen, char *buffer)`

Writes a string buffer to the display one character at a time.

- Automatically updates cursor position after each character.
- After completion, the next available position is stored in:
  - `screen->current_row`
  - `screen->current_column`
- Uses internal `setCursor()` and `buildBytes()` calls for each character.

---

### `void setCursor(uint8_t pos, uint8_t pcf_address)`

Moves the display cursor to a specific DDRAM address.

- `pos` is the raw DDRAM address, not `(row, column)`.
- To position using logical coordinates, use `getAddress(row, column)`.

---

### `uint8_t getAddress(uint8_t row, uint8_t column)`

Returns the DDRAM address corresponding to a `(row, column)` position.

#### 20×4 Address Map

