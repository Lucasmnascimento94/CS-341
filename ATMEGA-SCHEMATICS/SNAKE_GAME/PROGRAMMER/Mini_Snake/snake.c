#include "snake.h"

// ============================================================================
// LOW-LEVEL PIN CONTROL FUNCTIONS
// ============================================================================

// Purpose: Sets the LED data pin HIGH (to 3.3V or 5V)
// This is the most basic function - it directly controls the hardware pin
// Used when we need to send a HIGH signal to the LED
void DIN_H(void) {
    LED_PORT |= (1 << LED_PIN);
}

// Purpose: Sets the LED data pin LOW (to 0V / ground)
// This is the opposite of DIN_H() - it turns the pin OFF
// Used when we need to send a LOW signal to the LED
void DIN_L(void) { 
    LED_PORT &= ~(1<<LED_PIN); 
}

// ============================================================================
// TIMING FUNCTION
// ============================================================================

// Purpose: Creates a precise delay by waiting for a specific number of CPU cycles
// Parameter 'c': Number of clock cycles to wait
// This is crucial for WS2812 LEDs because they require very precise timing
// The microcontroller runs at 16 MHz, so each cycle takes 1/16,000,000 seconds
// This function uses a built-in AVR compiler function for accurate timing
void dcy(uint16_t c) {
    __builtin_avr_delay_cycles(c);
}

// ============================================================================
// BIT-LEVEL TRANSMISSION FUNCTIONS
// ============================================================================

// Purpose: Sends a single "0" bit to the WS2812 LED
// WS2812 LEDs encode data by how long the signal stays HIGH vs LOW
// For a "0" bit:
//   1. Set pin HIGH for T0H cycles (short high pulse)
//   2. Set pin LOW for T0L cycles (long low pulse)
// This creates a specific waveform pattern that the LED recognizes as "0"
void writeZero(void) { 
    DIN_H();           // Turn pin HIGH
    dcy(T0H);          // Keep HIGH for T0H cycles (short time ~0.4 microseconds)
    DIN_L();           // Turn pin LOW
    dcy(T0L);          // Keep LOW for T0L cycles (long time ~0.85 microseconds)
}

// Purpose: Sends a single "1" bit to the WS2812 LED
// For a "1" bit:
//   1. Set pin HIGH for T1H cycles (long high pulse)
//   2. Set pin LOW for T1L cycles (short low pulse)
// This creates a different waveform pattern that the LED recognizes as "1"
// The HIGH pulse is longer for "1" than for "0"
void writeOne(void) { 
    DIN_H();           // Turn pin HIGH
    dcy(T1H);          // Keep HIGH for T1H cycles (long time ~0.8 microseconds)
    DIN_L();           // Turn pin LOW
    dcy(T1L);          // Keep LOW for T1L cycles (short time ~0.45 microseconds)
}

// ============================================================================
// BYTE-LEVEL TRANSMISSION FUNCTION
// ============================================================================

// Purpose: Sends one complete byte (8 bits) to the WS2812 LED
// Parameter 'b': The byte value to send (0-255)
// 
// How it works:
// - Breaks the byte into 8 individual bits
// - Sends each bit one at a time, starting with the most significant bit (bit 7)
// - For each bit, calls either writeOne() or writeZero() depending on the bit's value
// 
// Example: If b = 170 (binary: 10101010), it sends: 1,0,1,0,1,0,1,0
void send_byte(uint8_t b){
    // Loop through each bit position from bit 7 (leftmost) down to bit 0 (rightmost)
    // m starts at 0x80 (binary: 10000000) which masks bit 7
    // Each iteration, m shifts right by 1 (m >>= 1) to check the next bit
    // Loop continues as long as m is non-zero (hasn't shifted all the way to 0)
    for (uint8_t m = 0x80; m; m >>= 1) {
        // Check if the current bit in byte 'b' is 1 or 0
        // (b & m) performs a bitwise AND to test if that specific bit is set
        // If the bit is 1: call writeOne() to send the "1" pulse pattern
        // If the bit is 0: call writeZero() to send the "0" pulse pattern
        (b & m) ? writeOne() : writeZero();
    }
}

// ============================================================================
// PIXEL-LEVEL FUNCTIONS
// ============================================================================

// Purpose: Sends color data for a single LED pixel
// Parameters: g (green), r (red), b (blue) - each is a value from 0-255
//             Note: The parameter is named 'b' for blue, don't confuse with the byte 'b' in send_byte()
// 
// Important: WS2812 LEDs expect colors in GRB order (Green, Red, Blue), NOT RGB!
// This is why we send green first, then red, then blue
// 
// How it works:
// - Takes the three color values (G, R, B)
// - Sends each color value as one byte using send_byte()
// - The LED receives all three bytes and sets its color accordingly
void send_pixel(uint8_t g, uint8_t r, uint8_t b){ // GRB order
    send_byte(g);  // Send green value (0-255)
    send_byte(r);  // Send red value (0-255)
    send_byte(b);  // Send blue value (0-255)
}

// Purpose: Turns off all LEDs on the LED strip/matrix
// 
// How it works:
// - Loops through every LED position in the grid (ROWS × COLUMNS = GRID total LEDs)
// - For each LED, sends black color (0, 0, 0) which turns the LED off
// - This effectively clears the entire display
void clear(){
    // Loop through all LED positions in the grid
    // GRID = ROWS × COLUMNS = 8 × 16 = 128 LEDs
    for(int i = 0; i< (int)GRID; i++){
        // Send black color (0 red, 0 green, 0 blue) to turn off this LED
        send_pixel(0x00, 0x00, 0x00);      
    }
}

