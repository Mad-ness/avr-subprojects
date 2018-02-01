#include <avr/io.h>
#include <util/delay.h>
//#include <avr/pgmspace.h>
#include <avr/interrupt.h>
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


#define PIN_LED PB1
// #define PIN_PB0 PB0 // PCINT0 vector uses it
#define PIN_BUTTON PB2
// PB0 is the PCINT0 interrupt is allocated to RF433 receiver
// PB3 & PB4 occupied by TM1637 display
// #define PIN_TRANSMITTER PB4

#define led_on()     sbi(PORTB, PIN_LED)
#define led_off()    cbi(PORTB, PIN_LED)

//static uint8_t i = 0;
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

void display_error() {
    uint8_t letter_E = 1 + 20 + 10 + 40 + 8;
    uint8_t letter_r = 40 + 10 + 8;
    uint8_t letter_o = 10 + 40 + 4 + 8; 
    TM1637_display_digit(TM1637_SET_ADR_00H, letter_E);
    TM1637_display_digit(TM1637_SET_ADR_01H, letter_r);
    TM1637_display_digit(TM1637_SET_ADR_02H, letter_r);
    TM1637_display_digit(TM1637_SET_ADR_03H, letter_o);
}


void setup(void) {
//    if ( F_CPU == 16000000 ) clock_prescale_set(clock_div_1);
    attiny_init();
    TM1637_init();
    RCTSwitch_setup();
    DDRB |= (1 << PIN_LED) | ( 1 << PB2 );
    PORTB &= ~( 1 << PB2 );
}

uint8_t counter = 250;
uint8_t sender_id = 0x0E;
static uint8_t rec_data = 0;
//uint16_t seconds = 0;
//uint16_t with_data = 0;
void loop(void) {
    if (millis() % 1000) {
        if ( RCTSwitch_available() ) {
            uint8_t sender_byte = RCTSwitch_getValue();

            // checking whether this byte is a SENDER_ID byte
	    if ((0xF0 & sender_byte) == 0x50 && (0x0F & sender_byte) == sender_id) {
                RCTSwitch_reset();
                while ( ! RCTSwitch_available() ) { }
		uint8_t data = RCTSwitch_getValue();
                // ensure that is not a sender_id
		if (( 0xF0 & data ) != 0x50 ) {
                    RCTSwitch_reset(); 
                    display_test(data);
                }
            }


            if (rec_data == 0) {
               rec_data = 1;
	       PORTB |= ( 1 << PB2 );
            } else { 
               rec_data = 0;
	       PORTB &= ~( 1 << PB2 );
            }
//            with_data += 1;
        } 
/*
        if ((seconds - with_data) > 60) {
	    display_error();
            seconds = with_data = 0;
        }
	seconds += 1;
*/
    }
}

void main(void) {
    setup();
    asm("sei");
    for (;;) {
        loop();
    }
}

