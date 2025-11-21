# Screen Driver Reference

## Data Types

### `SCREEN`

Holds the runtime state of a display device connected through a PCF8574 I/O expander.  
Tracks the current cursor position and configuration used during I²C communication.

|      Fields      |     Type      |                                          Description                                                |
|------------------|---------------|-----------------------------------------------------------------------------------------------------|
| `pcf8574_addr`   | `uint8_t`     | 7-bit I²C address of the screen’s PCF8574 expander.                                                 |
| `current_row`    | `uint8_t`     | Current row index where the next character will be written.                                         |
| `current_column` | `uint8_t`     | Current column index where the next character will be written.                                      |
| `conf`           | `SCREEN_CONF` | Configuration struct that defines display mode, cursor, and format settings.                        |

---

### `SCREEN_CONF`

Defines the display configuration bits for entry mode, cursor, display, and function settings.  
These fields map directly to the HD44780 instruction set.

|     Fields     |    Type   |                                              Description                                                  |
|----------------|-----------|-----------------------------------------------------------------------------------------------------------|
| `I_D`, `SH`    | `uint8_t` | Entry mode bits — increment/decrement and shift configuration.                                            |
| `D`, `C`, `B`  | `uint8_t` | Display control bits — display on/off, cursor visibility, and blink.                                      |
| `S_C`, `R_L`   | `uint8_t` | Cursor/Display shift control bits.                                                                        |
| `DL`, `N`, `F` | `uint8_t` | Function set bits — data length, number of display lines, and font type.                                  |

### `LCDTile`

Represents a single custom LCD tile for use with CGRAM and DDRAM helpers

| Fields       | Type      |                                                Description                                                  |
|--------------|-----------|-------------------------------------------------------------------------------------------------------------|
| `pattern[8]` | `uint8_t` | The 8‑row bitmap defining the tile’s shape - Each entry uses the lower 5 bits to represent pixels in a row (HD44780 custom characters are 5×8)                                                                                                      |
| `row`        | `uint8_t` | Logical row position on the LCD where this tile should be displayed.                                        |
| `column`     | `uint8_t` | Logical column position on the LCD where this tile should be displayed.                                     |
| `slot`       | `uint8_t` | CGRAM slot index 0–7 where the tile’s pattern is stored.                                                    |

---

## Methods

### `void screenInit(SCREEN *screen, bool default_conf)`

Initializes the display with either a default configuration or a custom one.

- If `default_conf == true`, the display is configured with default values (increment mode, display on, 2/4 lines).
- If `default_conf == false`, you must manually set `screen->conf` before calling this function.

---

### `void screenWriteDDRAM(SCREEN *screen, char *buffer)`

Writes a string buffer to the display one character at a time.

- Automatically updates cursor position after each character.
- After completion, the next available position is stored in:
  - `screen->current_row`
  - `screen->current_column`
- Uses internal `setCursor()` and `buildBytes()` calls for each character.

---

### `void setCursorDDRAM(uint8_t pos, uint8_t pcf_address)`

Moves the display cursor to a specific DDRAM address.

- `pos` is the raw DDRAM address, not `(row, column)`.
- To position using logical coordinates, use `getAddress(row, column)`.

---

### `uint8_t getAddress(uint8_t row, uint8_t column)`

Returns the DDRAM address corresponding to a `(row, column)` position.

---

### `void setCursorCGRAM(uint8_t pos, uint8_t pcf_address)`

Moves the display cursor to a specific CGRAM address.
At RAM_BIT = 6 

- To position using logical coordinates, use `getAddress(row, column)`

---

### `screenWriteCGRAM(SCREEN *screen, const uint8_t *pattern, uint8_t char_index)`

Writes a custom character pattern into the LCD’s CGRAM via I²C.
char_index - selects which CGRAM slot (0–7) to write the character into.

- To position using logical coordinates, use `getAddress(row, column)`

---

### `void screenWriteDDRAM(SCREEN *screen, char *buffer)`

Writes a string of characters into the LCD’s DDRAM via I²C.
buffer - a null‑terminated string to be written to the display.

- To position using logical coordinates, use `getAddress(row, column)`

---

### `void screenSetCursor(SCREEN *screen, uint8_t row, uint8_t column)`

Set cursor at specific position on the display screen.

- Use address of ``getAddress(row, column)` to set position to screen by `setCursorDDRAM(addr, screen->pcf8574_addr)`

---

### `void screenClear(SCREEN *screen)`

Clear/Reset the screen.

---

### `void screenWriteAt(SCREEN *screen, uint8_t row, uint8_t column, char *text)`

Write text character at specific position on the display screen.

- Set position to screen by `screenSetCursor(screen, row, column)` 
- Display text by `screenWriteDDRAM(screen, text)` 
---

### `void screenPrintCustomChar(SCREEN *screen, uint8_t row, uint8_t col, uint8_t char_index)`

Display custom character at specific position on the display screen.

- Set position to screen by `screenSetCursor(screen, row, column)` 

---

### `void loadTileBatch(SCREEN *screen, LCDTile *tiles, uint8_t count)`

Select the correct CGRAM slot based on the tile’s index.
Writes the 8‑row bitmap pattern into CGRAM for icons.

- Use  `screenWriteCGRAM(screen, pattern, char_index)`
- and `screenPrintCustomChar(screen, row, col, char_index)`

---

### `void snakeGameIcon(SCREEN *screen)`

Display Snake Dame icon by designed bitmap in decimal to the screen.

- Display each 8-bits decimal tile by `loadTileBatch(screen, tiles, count)`

---

### `void pacManIcon(SCREEN *screen)`

Display Pac-Man icon by designed bitmap in decimal to the screen.

- Display each 8-bits decimal tile by `loadTileBatch(screen, tiles, count)`

---

### `void pongIcon(SCREEN *screen)`

Display Pong icon by desined bitmap in decimal to the screen.

- Display each 8-bits decimal tile by `loadTileBatch(screen, tiles, count)`

---

### `void spacecraftIcon(SCREEN *screen)`

Display Spacecraft icon by designed bitmap in decimal to the screen.

- Display each 8-bits decimal tile by `loadTileBatch(screen, tiles, count)`

---

### `void updateGameIcon(SCREEN *screen, const char *game_name)`

Set icon to match with each game name string.

- Use `snakeGameIcon(screen)`, `pacManGameIcon(screen)`, `pongIcon(screen)`, `spacecraftIcon(screen)`

--

### `void updateGameScreen(SCREEN *screen, char *game_name, uint16_t current_score, uint16_t top_score)`

Display game, current score, top score, and icon of each game at their positions to the display screen.

- Display each element to specific position by `screenWriteAt(&screen, row, column, &text)`
- Matches icon to game by `updateGameIcon(screen, game_name)`

#### 20×4 Address Map

