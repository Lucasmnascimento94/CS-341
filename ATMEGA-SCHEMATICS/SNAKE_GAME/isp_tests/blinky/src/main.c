#include <avr/io.h>
#include <util/delay.h>

int main() {
    DDRD |= _BV(PD0);
    while (1) {
        PORTD &= ~_BV(PD0);
        _delay_ms(1000);
        PORTD |= _BV(PD0);
        _delay_ms(1000);
    }
}
