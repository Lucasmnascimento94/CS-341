#ifndef APA102_H
#define APA102_H

/*============================================================================
 *  APA102 GRID DRIVER - MULTI-TILE SUPPORT
 *============================================================================
 * Target MCU : ATmega168A (20MHz)
 * Author(s)  : Lucas Nascimento, Andres Nino.
 * Last update: 10/16/2025
 *============================================================================
 *
 *  Reference:
 *    - APA102-2020 SUPER LED:  ./datasheet/PIXEL/3587.pdf
 *    - APA102 FPS Analysis:  ./SNAKE_GAME/APA102_FPS_ANALYSIS.xlsx
 *    - SPI Application Note :  ./documentation/protocols/SPI.md
 *
 *  Description:
 *    - Defines configuration and public API to use the ATmega168A MCU via SPI
 *      to address "APA102-2020 SUPER LED" diodes inside a snake-connected,
 *      multi-tile grid system of RGB pixels.
 *    - Serpentine (“snake”) means even rows go L→R, odd rows go R→L inside
 *      each tile.
 *
 *============================================================================*/

#include "includes.h"
#include "spi.h"
#include "start.h"

/*============================================================================
 *  1) PROJECT CONFIG
 *===========================================================================*/

/* size of ONE tile (in LEDs)*/
#ifndef TILE_W
#   define TILE_W 14
#endif
#ifndef TILE_H
#   define TILE_H 14
#endif
#define TILE_PIXELS (TILE_W * TILE_H)

/* tile grid size (tiles across x tiles down) */
#ifndef TILE_COLUMNS
#   define TILE_COLUMNS 1
#endif
#ifndef TILE_ROWS
#   define TILE_ROWS 1
#endif
#define TILE_COUNT (TILE_COLUMNS * TILE_ROWS)

/* size of ENTIRE grid (in LEDs) */
#define GRID_W (TILE_COLUMNS * TILE_W)
#define GRID_H (TILE_ROWS * TILE_H)
#define GRID_PIXELS (GRID_W * GRID_H)

/* default APA102 brightness 0-31 (pixel header 0xE0|GB) */
#ifndef APA102_DEFAULT_GB
#   define APA102_DEFAULT_GB 31
#endif

/*===========================================================================
 * 2) MAPPING HELPERS - 2D(x,y) -> linear index in SPI stream.
 *===========================================================================*/

/* Find linear tile index (0..TILE_COUNT-1) given (x,y) coordinates */
static inline uint16_t tileIndexFromXY(uint16_t x, uint16_t y)
{
    uint16_t tx = x / TILE_W; // tile x
    uint16_t ty = y / TILE_H; // tile y
    if (tx >= TILE_COLUMNS || ty >= TILE_ROWS) return 0; // out of bounds

    /* linear tile order */
    /* return (uint16_t)(ty * TILE_COLUMNS + tx); */

    /* serpentine across each row of tiles */
    if (ty & 1U) {
        // odd: right->left
        return (uint16_t)(ty * TILE_COLUMNS + ((TILE_COLUMNS - 1U) - tx));
    } else {
        // even: left->right
        return (uint16_t)(ty * TILE_COLUMNS + tx);
    }
}

/* find local 1D index (0..TILE_PIXELS-1) given (lx, ly) */
static inline uint16_t tileLocalIdx(uint8_t lx, uint8_t ly)
{
    if (lx >= TILE_W || ly >= TILE_H) return 0; // out of bounds
    uint16_t base = (uint16_t)ly * TILE_W;  // first pixel of row ly
    /* serpentine inside tile (reverse odd rows) */
    return (ly & 1U) ? (uint16_t)(base + (TILE_W - 1U - lx))
                     : (uint16_t)(base + lx);
}

/* find global (x, y) linear index in SPI stream (0..GRID_PIXELS-1) */
static inline uint32_t gridXYToLinear(uint16_t x, uint16_t y)
{
    if (x >= GRID_W || y >= GRID_H) return 0; // out of bounds
    uint16_t t   = tileMapIndex(x, y);
    uint8_t  lx  = (uint8_t)(x % TILE_W);
    uint8_t  ly  = (uint8_t)(y % TILE_H);
    return (uint32_t)t * (uint32_t)TILE_PIXELS + (uint32_t)tileLocalIdx(lx, ly);
}

/*===========================================================================
 *********************************** API ************************************
 *===========================================================================*/
void apa102Init(void);
void gridSetBrightness(uint8_t gb);
void gridClear(uint8_t r, uint8_t g, uint8_t b);           // fill framebuffer
void gridSetXY(uint16_t x, uint16_t y,                       // update one pixel
                 uint8_t r, uint8_t g, uint8_t b);          
void gridSetRowRGB(uint16_t y, const uint8_t *rgb_line);     // update a whole row
void gridShow(uint8_t brightness);                          // stream all pixels over SPI

#endif
