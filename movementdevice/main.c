#define F_CPU                   16000000UL
#include <avr/io.h>
#include <util/delay.h>
#define PIN_PHOTOSENSOR 	PB0
#define PIN_MOVEMENTSENSOR 	PB1

#define PIN_LED_PHOTO		PB2
#define PIN_LED_MOVEMENT	PB3

#define pin_off(pin)            (PORTB&=~(1<<pin))
#define pin_on(pin)             (PORTB|=(1<<pin))


void main() {
    DDRB |= ( 1<<PIN_LED_PHOTO ) | ( 1<<PIN_LED_MOVEMENT );
    while ( 1 ) {

        if ( PORTB & (1 << PIN_PHOTOSENSOR) == 0 ) {
            pin_off(PIN_LED_PHOTO);
        } else {
            pin_on(PIN_LED_PHOTO);
        }
	
        if ( PORTB & (1 << PIN_MOVEMENTSENSOR) == 0 ) {
            pin_off(PIN_LED_MOVEMENT);
        } else {
            pin_on(PIN_LED_MOVEMENT);
        }
//        _delay_ms(100);
    } 
}

