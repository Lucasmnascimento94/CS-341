#include "apa102.h"
#include "spi.h"
#include <string.h>

/*============================================================================
 *  APA102 GRID DRIVER - MULTI-TILE SUPPORT - IMPLEMENTATION
 *============================================================================
 * Target MCU : ATmega168A (20MHz)
 * Author(s)  : Lucas Nascimento, Andres Nino.
 * Last update: 12/10/2025
 *============================================================================
 *  Uses hardware SPI through spi.c:
 *    - spiInitPoll()
 *    - spiWritePoll_()
 *
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  HELPER FUNCTIONS
 *----------------------------------------------------------------------------*/

static inline uint8_t apa102_brightness_header(void)
{
    /* 0b111xxxxx, xxxxx = global brightness (0–31) */
    return (uint8_t)(0xE0 | (APA102_GLOBAL_BRIGHTNESS & 0x1F));
}

/*----------------------------------------------------------------------------*
 *  PUBLIC DRIVER API
 *----------------------------------------------------------------------------*/

void apa102_init(void)
{
    /* Configure SPI in polling mode; prescaler etc. from spi.h */
    spiInitPoll();
}

/* Clear framebuffer (all LEDs off). */
void apa102_fb_clear(uint8_t *fb)
{
    if (!fb)
        return;
    memset(fb, 0, APA102_FB_BYTES);
}

/* Fill framebuffer with solid RGB color. */
void apa102_fb_fill(uint8_t *fb,
                    uint8_t r, uint8_t g, uint8_t b)
{
    if (!fb)
        return;

    for (uint32_t idx = 0; idx < APA102_TOTAL_PIXELS; ++idx)
    {
        APA102_FB_R(fb, idx) = r;
        APA102_FB_G(fb, idx) = g;
        APA102_FB_B(fb, idx) = b;
    }
}

/* Set one pixel by (i,j,k), 1-based index. */
void apa102_fb_set_ijk(uint8_t *fb,
                       uint8_t i, uint8_t j, uint8_t k,
                       uint8_t r, uint8_t g, uint8_t b)
{
    if (!fb)
        return;

    /* Range checks: 1-based coordinates inside the tile and tile chain. */
    if (i < 1 || i > APA102_TILE_W)
        return;
    if (j < 1 || j > APA102_TILE_H)
        return;
    if (k < 1 || k > APA102_NUM_TILES)
        return;

    uint32_t idx = APA102_INDEX_IJK(i, j, k);
    if (idx >= APA102_TOTAL_PIXELS)
        return;

    APA102_FB_R(fb, idx) = r;
    APA102_FB_G(fb, idx) = g;
    APA102_FB_B(fb, idx) = b;
}

/* Set one pixel by linear index (0-based). */
void apa102_fb_set_index(uint8_t *fb,
                         uint32_t idx,
                         uint8_t r, uint8_t g, uint8_t b)
{
    if (!fb)
        return;
    if (idx >= APA102_TOTAL_PIXELS)
        return;

    APA102_FB_R(fb, idx) = r;
    APA102_FB_G(fb, idx) = g;
    APA102_FB_B(fb, idx) = b;
}

/* Push framebuffer contents to APA102 serpentine*/
void apa102_flush(const uint8_t *fb)
{
    if (!fb)
        return;

    /* Start frame: 4 bytes of 0x00. */
    uint8_t start_frame[4] = {0x00, 0x00, 0x00, 0x00};

    /* End frame: at least (N/2) bits of ones; rule of thumb is
       ceil(NUM_PIXELS / 16) bytes. Generate them in small chunks. */
    uint32_t end_bytes_total = (APA102_TOTAL_PIXELS + 15u) / 16u;
    uint8_t end_buf[8];
    for (uint8_t i = 0; i < sizeof(end_buf); ++i)
    {
        end_buf[i] = 0xFF;
    }

    uint8_t hdr = apa102_brightness_header();
    uint8_t led_frame[4];

    START_SPI;

    /* Start frame */
    spiWritePoll_(start_frame, sizeof(start_frame));

    /* LED frames */
    for (uint32_t idx = 0; idx < APA102_TOTAL_PIXELS; ++idx)
    {
        uint8_t r = APA102_FB_R(fb, idx);
        uint8_t g = APA102_FB_G(fb, idx);
        uint8_t b = APA102_FB_B(fb, idx);

        led_frame[0] = hdr;
        led_frame[1] = b;
        led_frame[2] = g;
        led_frame[3] = r;

        spiWritePoll_(led_frame, 4);
    }

    /* End frame */
    while (end_bytes_total > 0)
    {
        uint16_t chunk = (end_bytes_total > sizeof(end_buf))
                             ? sizeof(end_buf)
                             : (uint16_t)end_bytes_total;
        spiWritePoll_(end_buf, chunk);
        end_bytes_total -= chunk;
    }

    STOP_SPI;
}

/* Simple power helper:
 *  - Uses a private static framebuffer in internal SRAM.
 *  - Lights only (i=1,j=1,k=1) and clears all other pixels.
 */
void apa102_test_first(uint8_t r, uint8_t g, uint8_t b)
{
    static uint8_t fb[APA102_FB_BYTES];

    apa102_fb_clear(fb);
    apa102_fb_set_ijk(fb, 1, 1, 1, r, g, b);
    apa102_flush(fb);
}
