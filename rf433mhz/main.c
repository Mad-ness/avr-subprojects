#define F_CPU 1200000UL
#include <stdint.h>
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// https://github.com/rustam-iskenderov/ATtiny13-transmitter-433Mghz


#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define PIN_LED PB3

void setup(void) {
    //PORTB = (1<<PIN_LED) | (1<<PB3);
    DDRB = (1<<PIN_LED) | (1<<DDB3);
}

void loop(void) {
    sbi(PORTB, PIN_LED);
    _delay_ms(500);
    cbi(PORTB, PIN_LED);
    _delay_ms(500);
}

void main(void)
{
    setup();
    for (;;) {
        loop();
    }; 
}

