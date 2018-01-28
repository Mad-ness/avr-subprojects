#include <avr/io.h>
#include <util/delay.h>
//#include <avr/pgmspace.h>
#include <avr/power.h>
#include "tm1637.h" // Pins CLK and DIO are predefined, see the tm1637.h file
#include "rctswitch.h"
#include "common.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


#define PIN_LED PB4
// PB0 is the PCINT0 interrupt is allocated to RF433 receiver
// PB1 & PB2 occupied by TM1637 display
#define PIN_TRANSMITTER PB4

static uint8_t i = 0;

/*
ISR(PCINT0_vect) {
  RCTSwitch_interruptHandler();
}
*/

void display_test(const uint8_t number) {
    int d0 = number / 1000;
    int d1 = (number - d0*1000) / 100;
    int d2 = (number - d0*1000 - d1*100)/10;
    int d3 = (number - d0*1000 - d1*100 - d2*10);
    TM1637_display_digit(TM1637_SET_ADR_00H, d0);
    TM1637_display_digit(TM1637_SET_ADR_01H, d1);
    TM1637_display_digit(TM1637_SET_ADR_02H, d2);
    TM1637_display_digit(TM1637_SET_ADR_03H, d3);

    //TM1637_display_colon(true);
    //_delay_ms(200);
    //TM1637_display_colon(false);
}


void setup(void) {
    if ( F_CPU == 16000000 ) clock_prescale_set(clock_div_1);
    attiny_init();
    RCTSwitch_setup();
    TM1637_init();
    DDRB |= (1 << PIN_LED);
}

uint8_t counter = 250;
void loop(void) {
    if ( RCTSwitch_available() ) {
        uint8_t value = RCTSwitch_getValue();
        RCTSwitch_reset();
        display_test(value);
    }
    
    if (millis() % 1000 == 0) { 
    //    display_test(counter);        
	sbi(PORTB, PIN_LED);
	_delay_ms(200);
	cbi(PORTB, PIN_LED);
//	counter -= 10;
//	if (counter < 10) { 
//           counter = 250;
//        }
    }
}

void main(void) {
    setup();
    for (;;) {
        loop();
    }
}

