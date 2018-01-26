#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define PIN_LED PB3
#define PIN_TX  PB4

void setup(void) {
    DDRB |= (1<<PIN_LED);
}

void loop(void) {
    sbi(PORTB, PIN_LED);
    _delay_ms(1000);
    cbi(PORTB, PIN_LED);
    _delay_ms(1000);
}

void main(void) {
    setup();
    for (;;) {
        loop();
    }
}

