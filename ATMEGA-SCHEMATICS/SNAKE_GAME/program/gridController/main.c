#include "start.h"
#include "uart.h"
/*
DDRX -> DIRECTION
*/
#define max 220


#define W 14
#define H 14
#define SCALE 2                // 7x7 -> 14x14
#define BRIGHT 0x03
#define BKG_R 0
#define BKG_G 0
#define BKG_B 0
#define PAC_B 0xFF
#define PAC_G 0x00
#define PAC_R 0x00

extern void sendZero(void);
extern void sendOne(void);

static inline void send8Zeros(void){ for(int i=0;i<8;i++) sendZero(); }
static inline void send8Ones(void) { for(int i=0;i<8;i++) sendOne();  }


static inline void sendByte(uint8_t v){
    for (int b = 7; b >= 0; --b) (v & (1u<<b)) ? sendOne() : sendZero();
}

static inline void sendBrightMin(void){ sendByte(0xE1); }

static inline void sendPixel_BGR(uint8_t B, uint8_t G, uint8_t R){
    sendBrightMin();                // global brightness (min)
    // B:
    for(int b=7;b>=0;--b) (B & (1u<<b)) ? sendOne() : sendZero();
    // G:
    for(int b=7;b>=0;--b) (G & (1u<<b)) ? sendOne() : sendZero();
    // R:
    for(int b=7;b>=0;--b) (R & (1u<<b)) ? sendOne() : sendZero();
}


static const uint8_t PAC7_OPEN[7][7] = {
    {0,0,1,1,1,0,0},
    {0,1,1,1,1,1,0},
    {1,1,1,1,1,1,1},
    {1,1,1,0,0,0,0},
    {1,1,1,1,1,0,0},
    {0,1,1,1,1,0,0},
    {0,0,1,1,1,0,0},
};
// Closed (circle)
static const uint8_t PAC7_CLOSED[7][7] = {
    {0,0,1,1,1,0,0},
    {0,1,1,1,1,1,0},
    {1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1},
    {0,1,1,1,1,1,0},
    {0,0,1,1,1,0,0},
};


static inline int phys_x_for_row(int x, int y) {
    return (y & 1) ? (W - 1 - x) : x;
}
static void draw_pacman_frame_at(int offx, int offy, const uint8_t sprite[7][7]) {
    // Start frame: 32 zero bits
    for (int i=0;i<32;i++) sendZero();

    for (int y=0; y<H; ++y) {
        // serpentine: even rows left->right, odd rows right->left
        int x_start = (y & 1) ? (W-1) : 0;
        int x_end   = (y & 1) ? -1    : W;
        int x_step  = (y & 1) ? -1    : 1;

        for (int x = x_start; x != x_end; x += x_step) {
            int sx = x - offx;
            int sy = y - offy;
            uint8_t on = 0;
            if (sx >= 0 && sx < 7 && sy >= 0 && sy < 7) {
                on = sprite[sy][sx];
            }
            if (on) {
                sendPixel_BGR(PAC_B, PAC_G, PAC_R);   // Pac-Man pixel
            } else {
                sendPixel_BGR(0x00, 0x00, 0x00);      // background
            }
        }
    }

    // Robust end frame: at least (N+15)/16 bytes of 0xFF. You used “32 ones”; keep it:
    for (int i=0;i<32;i++) sendOne();
}

void pacman_walk_border(void) {
    // Pac-Man’s top-left placement window in a 14x14 for a 7x7 sprite is 0..7
    const int MIN = 0;
    const int MAX = W - 7; // 7

    int x = MIN, y = MIN;      // start top-left (inside the border)
    int phase = 0;             // 0: move right, 1: move down, 2: move left, 3: move up
    bool mouth_open = true;

    for (;;) {
        // pick sprite (toggle mouth)
        const uint8_t (*spr)[7] = mouth_open ? PAC7_OPEN : PAC7_CLOSED;
        mouth_open = !mouth_open;

        // draw at current position
        draw_pacman_frame_at(x, y, spr);

        // small pace delay (tweak as needed)
        _delay_ms(105);

        // advance along the perimeter
        switch (phase) {
            case 0: // → along top edge (y = MIN), x: MIN..MAX
                if (x < MAX) x++;
                else phase = 1;
                break;
            case 1: // ↓ along right edge (x = MAX), y: MIN..MAX
                if (y < MAX) y++;
                else phase = 2;
                break;
            case 2: // ← along bottom edge (y = MAX), x: MAX..MIN
                if (x > MIN) x--;
                else phase = 3;
                break;
            case 3: // ↑ along left edge (x = MIN), y: MAX..MIN
                if (y > MIN) y--;
                else phase = 0;
                break;
        }
    }
}


void sendOne(){
    PORTC |= (1<<PC5) | (1<<PC4);
    _delay_us(1);
    PORTC &= ~(1<<PC5);
    _delay_us(1);
}

void sendZero(){
    PORTC &= ~(1<<PC4);
    PORTC |= (1<<PC5);
    _delay_us(1);
    PORTC &= ~(1<<PC5);
    _delay_us(1);
}

void sendBright(){
    sendOne();
    sendOne();
    sendOne();

    sendZero();
    sendZero();
    sendZero();
    sendOne();
    sendOne();
    
}
void test(){
    for(int i=0; i<32;i++){
        sendZero();
    }

    for(int i=0; i<max; i++){
        sendBright();
        for(int j=0; j<8; j++){
            sendOne();
        }
        for(int j=0; j<8; j++){
            sendZero();
        }
        for(int j=0; j<8; j++){
            sendZero();
        }
    }

    for(int i=0; i<32;i++){
        sendOne();
    }

      _delay_ms(1000);




      for(int i=0; i<32;i++){
        sendZero();
    }

    for(int i=0; i<max; i++){
        sendBright();
        for(int j=0; j<8; j++){
            sendZero();
        }
        for(int j=0; j<8; j++){
            sendOne();
        }
        for(int j=0; j<8; j++){
            sendZero();
        }
    }

    for(int i=0; i<32;i++){
        sendOne();
    }

      _delay_ms(1000);








      for(int i=0; i<32;i++){
        sendZero();
    }

    for(int i=0; i<max; i++){
        sendBright();
        for(int j=0; j<8; j++){
            sendZero();
        }
        for(int j=0; j<8; j++){
            sendZero();
        }
        for(int j=0; j<8; j++){
            sendOne();
        }
    }

    for(int i=0; i<32;i++){
        sendOne();
    }

      _delay_ms(1000);

    for(int i=0; i<32;i++){
        sendZero();
    }
    
    for(int i=0; i<max; i++){
        sendBright();
        for(int j=0; j<24; j++){
            sendZero();
        }
    }
    for(int i=0; i<32;i++){
        sendOne();
    }
}


int main(void){

    DDRC |= (1<<PC5) | (1<<PC4);
    LED_DDR |= (1 << LED_PIN);  
    //DDRB |= (1<<PB1);
    //PORTB &= ~(1<<PB1);
    //seed_prng();
    gpioConfig();


    char c[30];
    sprintf(c, "HELLO WORLD SNAKE PROJECT");
    uartWrite_(c);
    _delay_ms(100);
    while(1){
        pacman_walk_border();
        //draw_pacman_frame(3,3,1);
         //_delay_ms(1000);
        //test();
        //uartWrite("lOOPING\n", 9);
       // _delay_ms(300);
    }
}
