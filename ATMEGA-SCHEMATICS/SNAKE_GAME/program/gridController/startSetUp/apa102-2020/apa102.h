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

/* Comentator: Lucas Nascimento
The header file is used for declaring functions that will be accessed by other files... it the equivalent
of "where your public functions are exposed to the callers".. you do not execute logic in header files.

*/
/*===========================================================================
 * 2) MAPPING HELPERS - 2D(x,y) -> linear index in SPI stream.
 *===========================================================================*/

/* Find linear tile index (0..TILE_COUNT-1) given (x,y) coordinates */
static inline uint16_t tileIndexFromXY(uint16_t x, uint16_t y)
{
    uint16_t tx = x / TILE_W; // tile x
    uint16_t ty = y / TILE_H; // tile y


/* Commentator: Lucas Nascimento
-> tx = x/TILE_W (tx >= TILE_COLUMNS will probably always false becuase even if you try to access
a coordinate greater than the width, x/TILE_W will be smaller than width size... it Will not protect against
the out of bounds error... Try to check if x>=TILE_W at the begining of the function and y >= TILE_H)


-> if x and y are the coordinates, the return statement is also not returning the correct value..
    0 < ty * TILE_COLUMNS  < TILE_COLUMNS

-> the reuturn statement is not considering the pixels you walked through when you jump to the next row..
    it should be a multiplacation.. I see that ty seems to be trying to do that but it is actually a double < 1.

    The grid will work with this pattern:
    i
j   (1,1).................................(w,1) 
      .
      .
      .
      .
      .
      .
    (1,h).................................(w,h)

    in the odd rows the address increases to the right >>
    in the even rows the address increases to the left <<
    so you need two functions, one for odd rows and another for even rows.. (like what you are trying to do)

    so the function to get an address from i j in the odd rows would be:

    addr = i + (j-1)*w, where 1 <= i <= w and 1 <= y <= h
    so doing this patterns:
    
    First row:
    addr = i + (1-1)*w -> addr = i

    Third tow:
    addr = i + (3-1)*w -> addr = i + 2*w (so now it is counting the amount of pixels you already passed before the third row
    plus the pixels you passed in the current row...)
*/


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
