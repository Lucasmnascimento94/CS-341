#ifndef APA102_H
#define APA102_H

#include "includes.h"
#include "spi.h"
#include <stdint.h>

/*============================================================================
 *  APA102 GRID DRIVER - MULTI-TILE SUPPORT
 *============================================================================
 * Target MCU : ATmega168A (20MHz)
 * Author(s)  : Lucas Nascimento, Andres Nino.
 * Last update: 12/10/2025
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
 *
 *  * Design:
 *   - LED driver: no game logic.
 *   - Uses global SPI module (spiInitPoll, spiWritePoll_).
 *   - Supports one or more LxW serpentine tiles chained on MOSI/SCK.
 *   - Uses a simple RGB framebuffer (R,G,B bytes per pixel).
 *
 *  * Coordinate system:
 *   - i : column within tile,   1 <= i <= APA102_TILE_W
 *   - j : row within tile,      1 <= j <= APA102_TILE_H
 *   - k : tile index along chain, 1 <= k <= APA102_NUM_TILES
 *
 *   * Serpentine function per tile:
 *    - Odd rows  (j = 1,3,5,...) : address increases to the right >>
 *    - Even rows (j = 2,4,6,...) : address increases to the left  <<
 *    - Then offset by (k-1)*APA102_STRIDE for each tile in the chain.
 *============================================================================*/

/*----------------------------------------------------------------------------
 *  TILE GEOMETRY
 *---------------------------------------------------------------------------*/

/* Per-tile resolution (physical APA102-2020 matrix L x W). */
#define APA102_TILE_W 9
#define APA102_TILE_H 14

/* Number of chained tiles. */
#ifndef APA102_NUM_TILES
#define APA102_NUM_TILES 1
#endif

/* Pixels per tile and stride between tiles (in pixels). */
#define APA102_TILE_PIXELS (APA102_TILE_W * APA102_TILE_H)
#define APA102_STRIDE APA102_TILE_PIXELS
#define APA102_TOTAL_PIXELS (APA102_STRIDE * APA102_NUM_TILES)

/*----------------------------------------------------------------------------
 *  GLOBAL BRIGHTNESS
 *---------------------------------------------------------------------------*/
/*
 * APA102 frame header per LED:
 *   0b111xxxxx, where xxxxx is global brightness in [0..31].
 * This is ANDed with per-channel RGB values, not a replacement for them.
 */

#ifndef APA102_GLOBAL_BRIGHTNESS
#define APA102_GLOBAL_BRIGHTNESS 4 /* low brightness for bring-up */
#endif

/*----------------------------------------------------------------------------
 *  BASIC FRAMEBUFFER LAYOUT
 *---------------------------------------------------------------------------*/
/*
 * RGB framebuffer model:
 *   fb[3*idx + 0] = R
 *   fb[3*idx + 1] = G
 *   fb[3*idx + 2] = B
 *
 * The driver treats 'fb' linear over all tiles:
 *   idx in [0 .. APA102_TOTAL_PIXELS-1].
 */

#define APA102_FB_BYTES (APA102_TOTAL_PIXELS * 3)

#define APA102_FB_R(fb, idx) ((fb)[3 * (idx) + 0])
#define APA102_FB_G(fb, idx) ((fb)[3 * (idx) + 1])
#define APA102_FB_B(fb, idx) ((fb)[3 * (idx) + 2])

/*----------------------------------------------------------------------------
 *  SERPENTINE MAPPING (i,j,k -> linear index)
 *---------------------------------------------------------------------------*/
/*
 *  Odd rows  (j = 1,3,5,...):
 *      base_row = (j-1) * w
 *      offset   = (i-1)                // left -> right
 *      idx_tile = base_row + offset
 *
 *  Even rows (j = 2,4,6,...):
 *      base_row = (j-1) * w
 *      offset   = (w - i)              // right -> left
 *      idx_tile = base_row + offset
 *
 *  Then:
 *      idx = idx_tile + (k-1) * STRIDE
 *
 *  All indices returned are 0-based.
 */

#define APA102_INDEX_IJK(i, j, k)                                                                              \
    (((j) & 0x01) ? /* odd row: left -> right */                                                               \
         (((uint32_t)((j) - 1) * APA102_TILE_W) + ((uint32_t)(i) - 1) + ((uint32_t)((k) - 1) * APA102_STRIDE)) \
                  : /* even row: right -> left */                                                              \
         (((uint32_t)((j) - 1) * APA102_TILE_W) + (uint32_t)(APA102_TILE_W - (i)) + ((uint32_t)((k) - 1) * APA102_STRIDE)))

/*----------------------------------------------------------------------------
 *  PUBLIC DRIVER API
 *---------------------------------------------------------------------------*/

/* Initialize SPI module for APA102 use.
 *   - Calls spiInitPoll() (Mode, bit order, prescaler from spi.h).
 *   - Leaves CS high (STOP_SPI) after init.
 *   - Assumes DATA = MOSI, CLK = SCK as defined in spi.h.
 */
void apa102_init(void);

/* Clear framebuffer (all LEDs off).
 * 'fb' must point to APA102_FB_BYTES bytes.
 */
void apa102_fb_clear(uint8_t *fb);

/* Fill framebuffer with a solid RGB color. */
void apa102_fb_fill(uint8_t *fb,
                    uint8_t r, uint8_t g, uint8_t b);

/* Set one pixel by (i,j,k). Indices are 1-based; out-of-range is ignored. */
void apa102_fb_set_ijk(uint8_t *fb,
                       uint8_t i, uint8_t j, uint8_t k,
                       uint8_t r, uint8_t g, uint8_t b);

/* Set one pixel by linear index (0-based). */
void apa102_fb_set_index(uint8_t *fb,
                         uint32_t idx,
                         uint8_t r, uint8_t g, uint8_t b);

/* Push framebuffer to the APA102 chain.
 *
 * (using spiWritePoll_):
 *   - START_SPI (assert CS low, LEDs ignore CS)
 *   - 4 bytes start frame: 0x00 0x00 0x00 0x00
 *   - For idx = 0 .. APA102_TOTAL_PIXELS-1:
 *       send: 0xE0 | (APA102_GLOBAL_BRIGHTNESS & 0x1F)
 *             B, G, R   (from framebuffer)
 *   - End frame: enough 0xFF/0x00 clocks to latch last LEDs
 *   - STOP_SPI
 */
void apa102_flush(const uint8_t *fb);

/* Power helper:
 *   Light first pixel (i=1,j=1,k=1) with given color and clear all others.
 *   Useful for verifying wiring and serpentine mapping.
 */
void apa102_test_first(uint8_t r, uint8_t g, uint8_t b);

#endif /* APA102_H */