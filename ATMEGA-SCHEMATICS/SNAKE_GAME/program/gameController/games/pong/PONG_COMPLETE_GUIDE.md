# Pong Game Implementation Guide
## Complete Guide Based on Current Codebase

---

## 📋 Table of Contents

1. [System Architecture Overview](#system-architecture-overview)
2. [Codebase Structure](#codebase-structure)
3. [Key Systems Analysis](#key-systems-analysis)
4. [Implementation Steps](#implementation-steps)
5. [Code Templates](#code-templates)
6. [Integration Guide](#integration-guide)

---

## 🏗️ System Architecture Overview

### Hardware Platform
- **MCU**: ATmega328P @ 20 MHz
- **LED Grid**: 48×32 WS2812B RGB LEDs (1536 LEDs total)
- **LCD Display**: I2C-based character LCD (for scores)
- **Input**: 4-directional joystick (PORTD pins 2-5)
- **Storage**: External SRAM via SPI
- **Architecture**: Multi-MCU (gameController + gridController communicate via SRAM)

### File Structure
```
gameController/
├── includes/          # Header files organized by module
│   ├── snake/
│   ├── sram/
│   ├── ws2812b/
│   ├── protocols/
│   └── ...
├── src/              # Source files organized by module
│   ├── snake/
│   ├── sram/
│   ├── ws2812b/
│   └── ...
├── games/            # Game implementations
│   └── pong/
├── libC/             # Common libraries
└── main.c            # Main entry point
```

---

## 🔍 Codebase Structure

### Key Directories

**`includes/`** - All header files:
- `includes/snake/snake.h` - Snake game interface
- `includes/sram/shared_memory.h` - SRAM memory map and buffer functions
- `includes/ws2812b/WS2812B.h` - LED grid definitions
- `includes/protocols/` - SPI, I2C, UART protocols

**`src/`** - All source files:
- `src/snake/snake.c` - Snake game implementation (REFERENCE!)
- `src/sram/shared_memory.c` - Buffer and SRAM functions
- `src/ws2812b/WS2812B.c` - LED driver

**`games/`** - Game implementations:
- `games/pong/` - Your Pong game (to be created)

---

## 🔑 Key Systems Analysis

### 1. Buffer System (CRITICAL!)

The system uses a **frame buffer stored in SRAM**:

```c
// Write a pixel to the buffer (GRB format)
bufferWrite(uint8_t g, uint8_t r, uint8_t b, uint32_t index);

// Clear entire buffer
bufferClear();

// Display buffer to LED grid
displayGrid();
```

**Buffer Index Calculation:**
```c
// Convert (i, j) coordinates to buffer index
uint32_t index = PIXEL_ADDRESS(i, j);
bufferWrite(g, r, b, index);
```

**Important Notes:**
- Buffer is in **SRAM**, not RAM
- Each pixel = 3 bytes (G, R, B)
- Buffer size = `SCREEN_BUFFER_SIZE` (48×32 = 1536 pixels)
- Colors are in **GRB format** (Green, Red, Blue), not RGB!

### 2. LED Grid Addressing

```c
// PIXEL_ADDRESS macro converts (i, j) to buffer index
// i: column (0 to SCREEN_WIDTH-1)
// j: row (0 to SCREEN_HEIGHT-1)
uint32_t addr = PIXEL_ADDRESS(i, j);
```

**Grid Dimensions:**
- `SCREEN_WIDTH = 48` (columns)
- `SCREEN_HEIGHT = 32` (rows)
- Coordinates: (0,0) to (47, 31)

### 3. Shared Memory System

**SRAM Communication:**
- Multi-MCU system uses shared SRAM
- Game controller writes game state to SRAM
- Grid controller reads from SRAM
- Uses **Clear To Access (CTA)** signal on PC3

**Memory Map** (defined in `shared_memory.h`):
- Buffer storage
- Game state structures
- Commands from grid controller
- Score information

**Game Definition:**
```c
#define SNAKE 1
#define SPACE 2
#define PONG 3
#define PACMAN 4
#define GAME SNAKE  // Change to PONG for your game
```

### 4. Game Loop Structure

From `main.c`, the game loop follows this pattern:

```c
while(1){
    // Wait for Clear To Access signal from grid controller
    while(!((PINC >> PC3) & 0x01)){/*wait*/}
    
    // Resume SPI to access SRAM
    spiResume(&spi);
    
    // Read commands/update game state
    walk();  // or pongUpdate() for Pong
    
    // Display to LED grid
    displayGrid();
    
    // Pause SPI to release bus
    spiPause(&spi);
}
```

### 5. Input System

**Joystick Input:**
- UP: PD2 (PCINT18)
- DOWN: PD3 (PCINT19)
- LEFT: PD4 (PCINT20)
- RIGHT: PD5 (PCINT21)

Commands are stored in SRAM command structure and read via `getCommand()`.

### 6. Color System

Colors defined in `includes/ws2812b/colors.h`:
```c
#define COLOR_WHITE   0xFFFFFF
#define COLOR_RED     0x00FF00  // Note: GRB format!
#define COLOR_GREEN   0xFF0000
#define COLOR_BLUE    0x0000FF
// ... many more
```

**IMPORTANT**: Colors are in **GRB format**:
- To write: `bufferWrite((color>>16)&0xFF, (color>>8)&0xFF, color&0xFF, index)`

---

## 📝 Implementation Steps

### Step 1: Update Shared Memory Configuration

**File**: `includes/sram/shared_memory.h`

1. Change game definition:
```c
#define GAME PONG  // Change from SNAKE
```

2. Add Pong-specific structures in the `#elif (GAME == PONG)` section:

```c
#elif (GAME == PONG)

/*_______________Header Names______________*/
#define MAGIC_PONG_STATE      "PNGS"
#define MAGIC_PONG_SCORE      "PNSC"
#define MAGIC_COMMANDS        "COMM"

/*__________Base addresses Definition___________*/
#define MEMORY_PADDING        ((uint32_t)0x20u)

#pragma pack(push, 1)
struct PONG_STATE {
    uint8_t ball_x;
    uint8_t ball_y;
    int8_t ball_vx;        // Velocity X (signed)
    int8_t ball_vy;        // Velocity Y (signed)
    uint8_t left_paddle_y;  // Top Y position
    uint8_t right_paddle_y; // Top Y position
    uint8_t paddle_length;  // Paddle height
    uint16_t left_score;
    uint16_t right_score;
    bool game_active;
    uint16_t block_size;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PONG_SRAM_MAP {
    struct PONG_STATE state;
    struct COMMANDS cmd;
    struct SCORE score;
};
#pragma pack(pop)

extern struct PONG_SRAM_MAP pong_sram_map;

#endif
```

### Step 2: Create Pong Header File

**File**: `includes/pong/pong.h` (create new directory)

```c
#ifndef PONG_H
#define PONG_H

#include "includes.h"
#include "shared_memory.h"
#include "WS2812B.h"
#include "colors.h"
#include "start.h"

/*________COMMAND_OPCODES_______*/
#define PONG_UP      0x01
#define PONG_DOWN    0x02
#define PONG_STALL   0x00

/*________Game Constants_______*/
#define PADDLE_LENGTH 4
#define PADDLE_WIDTH 1
#define PADDLE_LEFT_X 2
#define PADDLE_RIGHT_X 45
#define BALL_SIZE 1
#define SCORE_LIMIT 5

#define BALL_COLOR COLOR_WHITE
#define LEFT_PADDLE_COLOR COLOR_GREEN
#define RIGHT_PADDLE_COLOR COLOR_RED

/*============================================================================================*
 * INITIALIZATION                                                                              *
 *============================================================================================*/
void pongInit(void);
void pongReset(void);

/*============================================================================================*
 * GAME LOOP                                                                                   *
 *============================================================================================*/
void pongUpdate(void);
void pongRender(void);
bool pongGameIsActive(void);

/*============================================================================================*
 * GAME LOGIC                                                                                  *
 *============================================================================================*/
void pongUpdateBall(void);
void pongUpdatePaddles(void);
void pongCheckCollisions(void);
void pongCheckScoring(void);
void pongUpdateAI(void);

/*============================================================================================*
 * RENDERING                                                                                   *
 *============================================================================================*/
void pongDrawBall(void);
void pongDrawPaddles(void);
void pongClear(void);

#endif
```

### Step 3: Create Pong Source File

**File**: `src/pong/pong.c` (create new directory)

```c
#include "pong.h"
#include "uart.h"
#include "shared_memory.h"

#if (GAME == PONG)

/* Static game state */
static struct PONG_STATE *pong_state;

/*============================================================================================*
 * INITIALIZATION                                                                              *
 *============================================================================================*/

void pongInit(void){
    pong_state = &pong_sram_map.state;
    
    // Initialize game state
    pong_state->ball_x = SCREEN_WIDTH / 2;
    pong_state->ball_y = SCREEN_HEIGHT / 2;
    pong_state->ball_vx = 1;
    pong_state->ball_vy = 1;
    pong_state->left_paddle_y = SCREEN_HEIGHT / 2 - PADDLE_LENGTH / 2;
    pong_state->right_paddle_y = SCREEN_HEIGHT / 2 - PADDLE_LENGTH / 2;
    pong_state->paddle_length = PADDLE_LENGTH;
    pong_state->left_score = 0;
    pong_state->right_score = 0;
    pong_state->game_active = true;
    
    pongClear();
    pongRender();
    displayGrid();
}

void pongReset(void){
    pong_state->ball_x = SCREEN_WIDTH / 2;
    pong_state->ball_y = SCREEN_HEIGHT / 2;
    pong_state->ball_vx = (rand() % 2) ? 1 : -1;
    pong_state->ball_vy = (rand() % 2) ? 1 : -1;
    pong_state->left_paddle_y = SCREEN_HEIGHT / 2 - PADDLE_LENGTH / 2;
    pong_state->right_paddle_y = SCREEN_HEIGHT / 2 - PADDLE_LENGTH / 2;
}

/*============================================================================================*
 * GAME LOOP                                                                                   *
 *============================================================================================*/

void pongUpdate(void){
    if(!pong_state->game_active) return;
    
    pongUpdatePaddles();
    pongUpdateBall();
    pongCheckCollisions();
    pongCheckScoring();
    pongUpdateAI();
}

void pongRender(void){
    pongClear();
    pongDrawPaddles();
    pongDrawBall();
}

bool pongGameIsActive(void){
    return pong_state->game_active;
}

/*============================================================================================*
 * GAME LOGIC                                                                                  *
 *============================================================================================*/

void pongUpdateBall(void){
    // Update position
    pong_state->ball_x += pong_state->ball_vx;
    pong_state->ball_y += pong_state->ball_vy;
    
    // Bounce off top/bottom walls
    if(pong_state->ball_y <= 0 || pong_state->ball_y >= SCREEN_HEIGHT - 1){
        pong_state->ball_vy = -pong_state->ball_vy;
        // Clamp to boundaries
        if(pong_state->ball_y <= 0) pong_state->ball_y = 0;
        if(pong_state->ball_y >= SCREEN_HEIGHT - 1) pong_state->ball_y = SCREEN_HEIGHT - 1;
    }
}

void pongUpdatePaddles(void){
    // Read command from SRAM (set by grid controller based on joystick)
    getCommand();
    uint8_t cmd = pong_sram_map.cmd.cmdID;
    
    // Update left paddle (player)
    switch(cmd){
        case PONG_UP:
            if(pong_state->left_paddle_y > 0){
                pong_state->left_paddle_y--;
            }
            break;
        case PONG_DOWN:
            if(pong_state->left_paddle_y + pong_state->paddle_length < SCREEN_HEIGHT){
                pong_state->left_paddle_y++;
            }
            break;
    }
}

void pongUpdateAI(void){
    // Simple AI: follow ball
    uint8_t paddle_center = pong_state->right_paddle_y + pong_state->paddle_length / 2;
    
    if(pong_state->ball_y < paddle_center && pong_state->right_paddle_y > 0){
        pong_state->right_paddle_y--;
    } else if(pong_state->ball_y > paddle_center && 
              pong_state->right_paddle_y + pong_state->paddle_length < SCREEN_HEIGHT){
        pong_state->right_paddle_y++;
    }
}

void pongCheckCollisions(void){
    // Check collision with left paddle
    if(pong_state->ball_x == PADDLE_LEFT_X + PADDLE_WIDTH){
        if(pong_state->ball_y >= pong_state->left_paddle_y && 
           pong_state->ball_y < pong_state->left_paddle_y + pong_state->paddle_length){
            pong_state->ball_vx = -pong_state->ball_vx;
            pong_state->ball_x = PADDLE_LEFT_X + PADDLE_WIDTH + 1;
            
            // Add angle based on hit position
            int8_t hit_offset = pong_state->ball_y - (pong_state->left_paddle_y + pong_state->paddle_length / 2);
            pong_state->ball_vy += (hit_offset > 0) ? 1 : -1;
            
            // Limit velocity
            if(pong_state->ball_vy > 2) pong_state->ball_vy = 2;
            if(pong_state->ball_vy < -2) pong_state->ball_vy = -2;
        }
    }
    
    // Check collision with right paddle
    if(pong_state->ball_x == PADDLE_RIGHT_X){
        if(pong_state->ball_y >= pong_state->right_paddle_y && 
           pong_state->ball_y < pong_state->right_paddle_y + pong_state->paddle_length){
            pong_state->ball_vx = -pong_state->ball_vx;
            pong_state->ball_x = PADDLE_RIGHT_X - 1;
            
            // Add angle based on hit position
            int8_t hit_offset = pong_state->ball_y - (pong_state->right_paddle_y + pong_state->paddle_length / 2);
            pong_state->ball_vy += (hit_offset > 0) ? 1 : -1;
            
            // Limit velocity
            if(pong_state->ball_vy > 2) pong_state->ball_vy = 2;
            if(pong_state->ball_vy < -2) pong_state->ball_vy = -2;
        }
    }
}

void pongCheckScoring(void){
    // Ball went off left edge
    if(pong_state->ball_x < 0){
        pong_state->right_score++;
        if(pong_state->right_score >= SCORE_LIMIT){
            pong_state->game_active = false;
        } else {
            pongReset();
        }
    }
    // Ball went off right edge
    else if(pong_state->ball_x >= SCREEN_WIDTH){
        pong_state->left_score++;
        if(pong_state->left_score >= SCORE_LIMIT){
            pong_state->game_active = false;
        } else {
            pongReset();
        }
    }
}

/*============================================================================================*
 * RENDERING                                                                                   *
 *============================================================================================*/

void pongClear(void){
    bufferClear();
}

void pongDrawBall(void){
    uint32_t color = BALL_COLOR;
    uint32_t addr = PIXEL_ADDRESS(pong_state->ball_x, pong_state->ball_y);
    bufferWrite((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, addr);
}

void pongDrawPaddles(void){
    uint32_t left_color = LEFT_PADDLE_COLOR;
    uint32_t right_color = RIGHT_PADDLE_COLOR;
    
    // Draw left paddle
    for(uint8_t i = 0; i < pong_state->paddle_length; i++){
        uint8_t y = pong_state->left_paddle_y + i;
        if(y >= SCREEN_HEIGHT) break;
        
        for(uint8_t w = 0; w < PADDLE_WIDTH; w++){
            uint8_t x = PADDLE_LEFT_X + w;
            uint32_t addr = PIXEL_ADDRESS(x, y);
            bufferWrite((left_color >> 16) & 0xFF, (left_color >> 8) & 0xFF, left_color & 0xFF, addr);
        }
    }
    
    // Draw right paddle
    for(uint8_t i = 0; i < pong_state->paddle_length; i++){
        uint8_t y = pong_state->right_paddle_y + i;
        if(y >= SCREEN_HEIGHT) break;
        
        for(uint8_t w = 0; w < PADDLE_WIDTH; w++){
            uint8_t x = PADDLE_RIGHT_X + w;
            uint32_t addr = PIXEL_ADDRESS(x, y);
            bufferWrite((right_color >> 16) & 0xFF, (right_color >> 8) & 0xFF, right_color & 0xFF, addr);
        }
    }
}

#endif
```

### Step 4: Update Makefile

**File**: `Makefile`

Update source and include directories:

```makefile
SRC_DIRS = \
  ./ \
  src \
  libC \
  src/protocols \
  src/screen \
  src/snake \
  src/sram \
  src/tim \
  src/ws2812b \
  src/pong          # ADD THIS LINE

INC_DIRS = \
  ./ \
  libC \
  includes/protocols \
  includes/ \
  includes/screen \
  includes/snake \
  includes/sram \
  includes/tim \
  includes/ws2812b \
  includes/pong     # ADD THIS LINE
```

### Step 5: Update Main Loop

**File**: `main.c`

Replace Snake-specific code with Pong:

```c
#include "pong.h"  // Add at top

// In main(), replace:
// initSnake();
// generateFood();
// with:
pongInit();

// In while(1) loop, replace:
// walk();
// with:
pongUpdate();
pongRender();
```

### Step 6: Implement Shared Memory Functions

You'll need to add Pong-specific shared memory functions in `src/sram/shared_memory.c`:

```c
#elif (GAME == PONG)

void sharedMemoryInit(){
    // Initialize Pong SRAM map
    // Similar to Snake implementation
}

// Add other helper functions as needed

#endif
```

---

## 🔧 Integration Checklist

- [ ] Change `#define GAME` to `PONG` in `shared_memory.h`
- [ ] Add Pong structures to `shared_memory.h`
- [ ] Create `includes/pong/pong.h`
- [ ] Create `src/pong/pong.c`
- [ ] Update Makefile with pong directories
- [ ] Update `main.c` to use Pong functions
- [ ] Implement shared memory initialization for Pong
- [ ] Test compilation
- [ ] Test on hardware

---

## 📚 Key Reference Files

1. **Snake Implementation**: `src/snake/snake.c` - Best reference for patterns
2. **Buffer Functions**: `src/sram/shared_memory.c` - `bufferWrite()`, `bufferClear()`
3. **LED Display**: `src/ws2812b/WS2812B.c` - `displayGrid()`, `displayClear()`
4. **Main Loop**: `main.c` - Game loop structure
5. **Memory Map**: `includes/sram/shared_memory.h` - SRAM structure

---

## ⚠️ Important Notes

1. **Color Format**: Always use GRB format (Green, Red, Blue), not RGB!
2. **Buffer Index**: Use `PIXEL_ADDRESS(i, j)` to convert coordinates
3. **SRAM Access**: Must wait for CTA signal before accessing
4. **Multi-MCU**: Grid controller reads commands from SRAM
5. **Coordinates**: Grid is 0-based (0 to 47, 0 to 31)

---

## 🐛 Common Issues

1. **Colors look wrong**: Remember GRB format, not RGB!
2. **Nothing displays**: Check bufferClear() and displayGrid() are called
3. **Coordinates wrong**: Verify PIXEL_ADDRESS macro usage
4. **Game doesn't update**: Check CTA signal waiting in main loop
5. **Collisions not working**: Check coordinate bounds carefully

---

**Good luck implementing Pong! 🏓**

