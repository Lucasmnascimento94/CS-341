#include "stm32f411xe.h"
#include <stddef.h>
#include <stdint.h>

#define PAD_WIDTH 16
#define PAD_HEIGHT 16
#define STRIDE 16

#define FCPU      100000000UL
#define CYCLE_NS (1000000000UL / F_CPU)

#define IO_WRITE_CYC 6

#define T1H_TOT 0 // TODO:
#define T1L_TOT 0 // TODO:
#define T0H_TOT 0 // TODO:
#define T0L_TOT 0 // TODO:
#define TLL     0 // TODO:

#define T1H (T1H_TOT - IO_WRITE_CYC)
#define T1L (T1L_TOT - IO_WRITE_CYC)
#define T0H (T0H_TOT - IO_WRITE_CYC)
#define T0L (T0L_TOT - IO_WRITE_CYC)

#define DATA_PORT      GPIOA->BSRR
#define DATA_SET_PIN   (1U << 0)
#define DATA_RESET_PIN (1U << 16)

static inline void DIN_H(void) {
  DATA_PORT |= DATA_SET_PIN;
}
static inline void DIN_L(void) {
  DATA_PORT |= DATA_RESET_PIN;
}
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wimplicit-function-declaration"
static inline void dcy(uint16_t c) { /* TODO */ }
#pragma clang diagnostic pop

static inline void writeZero(void) {
  DIN_H();
  if (T0H > 0) {
    dcy(T0H);
  }
  DIN_L();
  if (T0L > 0) {
    dcy(T0L);
  }
}
static inline void writeOne(void) {
  DIN_H();
  if (T1H > 0) {
    dcy(T1H);
  }
  DIN_L();
  if (T1L > 0) {
    dcy(T1L);
  }
}
static inline void latch(void) {
  DIN_L();
  dcy(TLL);
}

static inline size_t pixel_address(uint8_t i, uint8_t j, uint8_t k) {
  return j % 2 == 0 ? (j * PAD_WIDTH) + i
                    : (j * PAD_WIDTH) + (PAD_WIDTH - i - 1);
}

static inline void send_byte(uint8_t b) {
  for (uint8_t m = 0x80; m; m >>= 1)
    (b & m) ? writeOne() : writeZero();
}

static inline void send_pixel(uint8_t b, uint8_t r, uint8_t g) {
  send_byte(b);
  send_byte(r);
  send_byte(g);
}

void ws2812bInit(void) {
  // Enable clock access to GPIOA
  RCC->AHB1ENR |= (1U << 0);
  // Set PA0 as output
  GPIOA->MODER |= (1U << 0);
  GPIOA->MODER &= ~(1U << 1);
  // Set PA0 high
  GPIOA->BSRR |= (1U << 0);
}

typedef struct {
  uint8_t g;
  uint8_t r;
  uint8_t b;
} Color;

static Color grid[PAD_WIDTH][PAD_HEIGHT];

void test_grid(void) {
  for (uint8_t i = 0; i < PAD_WIDTH; ++i) {
    for (uint8_t j = 0; j < PAD_HEIGHT; ++j) {
      if (j % 2 == 0) {
        Color pixel = grid[i][j];
        send_pixel(pixel.g, pixel.r, pixel.b);
      } else {
        Color pixel = grid[PAD_WIDTH - i - 1][j];
        send_pixel(pixel.g, pixel.r, pixel.b);
      }
    }
  }
  latch();
}

int main(void) {
  ws2812bInit();

  while (1) {
    test_grid();
  }
}
