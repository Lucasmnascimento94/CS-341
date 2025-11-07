#ifndef COLORS_H
#define COLORS_H

#include <stdint.h>

/*
 * WS2812B GRB format
 * Format: 0xGGRRBB
 */
#define COLOR_BLACK         0x000000
#define COLOR_WHITE         0xFFFFFF
#define COLOR_RED           0x00FF00
#define COLOR_GREEN         0xFF0000
#define COLOR_BLUE          0x0000FF

#define COLOR_CYAN          0xFFFF00
#define COLOR_MAGENTA       0x00FFFF
#define COLOR_YELLOW        0xFF00FF
#define COLOR_ORANGE        0xA5FF00

/*
 * Shades of Gray
 */
#define COLOR_LIGHT_GRAY    0xD3D3D3
#define COLOR_GRAY          0x808080
#define COLOR_DARK_GRAY     0xA9A9A9
#define COLOR_SILVER        0xC0C0C0

/*
 * Shades of Red
 */
#define COLOR_MAROON        0x008000
#define COLOR_CRIMSON       0x14DC3C
#define COLOR_FIREBRICK     0x22B222
#define COLOR_INDIAN_RED    0x5CCD5C

/*
 * Shades of Green
 */
#define COLOR_LIME          0x00FF00
#define COLOR_FOREST_GREEN  0x8B2222
#define COLOR_DARK_GREEN    0x640000
#define COLOR_SEA_GREEN     0x8B2E57
#define COLOR_OLIVE         0x808000

/*
 * Shades of Blue
 */
#define COLOR_NAVY          0x000080
#define COLOR_MIDNIGHT_BLUE 0x191970
#define COLOR_DARK_BLUE     0x00008B
#define COLOR_ROYAL_BLUE    0x6941E1
#define COLOR_SKY_BLUE      0xCE87EB

/*
 * Common Pastels and Earth Tones
 */
#define COLOR_PINK          0xC0FFCB
#define COLOR_LIGHT_CORAL   0x80F080
#define COLOR_SALMON        0x80FA72
#define COLOR_GOLD          0xD7FF00
#define COLOR_CHOCOLATE     0x69D21E
#define COLOR_SADDLE_BROWN  0x458B13
#define COLOR_KHAKI         0xE6F08C
#define COLOR_BEIGE         0xF5F5DC

/*
 * Miscellaneous Colors
 */
#define COLOR_AQUAMARINE    0xFF7FD4
#define COLOR_TURQUOISE     0xE040D0
#define COLOR_TEAL          0x800080
#define COLOR_AZURE         0x7F00FF
#define COLOR_LAVENDER      0xE6E6FA
#define COLOR_VIOLET        0xEE82EE
#define COLOR_PURPLE        0x008080
#define COLOR_INDIGO        0x004B82
#define COLOR_GOLDENROD     0xA5DA20
#define COLOR_TOMATO        0x63FF47
#define COLOR_LIGHT_SLATE   0x887799

#endif /* COLORS_H */