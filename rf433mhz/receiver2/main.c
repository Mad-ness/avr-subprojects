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


#define unpack_senderId(x)  	( 0x7 & ( x >> 10 ))
#define unpack_intpart(x) 	( 0x3F & x )
#define unpack_realpart(x)      ( 0x0F & ( x >> 6 ))
#define unpack_minus(x)	        (( 0x1 & ( x >> 10 )) > 0 ) ? true : false



uint16_t packtemp(int8_t intpart, int8_t realpart, bool is_minus) {
    return ( intpart ) | ( realpart << 6 ) | ( is_minus << 10 );
}

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

/** 
  Format of the value variable:
  - bits 0-5 - an integer part of temperature (max is 63)
  - bits 6-9 - a real part of temperature (0-10 values)
  - bit 10 - if set to 1, the temperature is below zero
  - bits 11-13 - Sender ID
  - bit 14 - a control bit
  - bit 15 - not used
**/


void display_temp(const uint16_t value) {
    bool temp_below_0 = unpack_minus(value);
    uint8_t temp_int_part = unpack_intpart(value);
    uint8_t temp_real_part = unpack_realpart(value);

    if ( temp_below_0 ) {
        // display "-" minus
        TM1637_display_segments( TM1637_SET_ADR_03H, 0x1 | 0x2 | 0x20 | 0x40 | 0x08 );
    } else {
        // display "o" celsuise symbol
        TM1637_display_segments( TM1637_SET_ADR_03H, 0x1 | 0x2 | 0x20 | 0x40 );
    }

    uint8_t d0 = temp_int_part / 10;     
    uint8_t d1 = temp_int_part - d0*10;
    TM1637_display_digit(TM1637_SET_ADR_00H, d0);
    TM1637_display_digit(TM1637_SET_ADR_01H, d1);
    TM1637_display_digit(TM1637_SET_ADR_02H, temp_real_part);
    TM1637_display_colon( true );
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
    DDRB |= (1 << PIN_LED);
    DDRB &= ~( 1 << PIN_BUTTON );
    PORTB &= ~( 1 << PB2 ) | ( 1 << PIN_BUTTON );
}

uint8_t counter = 250;
uint8_t sender_id = 0x0E;
static uint8_t rec_data = 0;


enum DisplayState { Temp = 1, Setup = 2 };
enum DisplayState  display_state = Temp;
enum DisplayState old_display_state;

unsigned long last_time = 0;
unsigned long pressed_button_time = 0;

int8_t setup_value = 0;
int8_t setup_min_value = -2;
int8_t setup_max_value = 3;
bool colon_blink_state = false;


int8_t setup_intpart = -2;
int8_t setup_realpart = 0;
bool   setup_minus = true;

uint16_t data = 0;	// it stores received data

void loop(void) {

    if ( display_state == Setup && millis() % 300 == 0 ) {
        TM1637_display_colon( ! colon_blink_state );
        colon_blink_state = ! colon_blink_state;
    }


    if (millis() % 100 == 0) {

        // if we have pressed a button
        if ((( 1 << PIN_BUTTON ) & PINB ) > 0 ) {
            _delay_ms(150); // to skip multiple signals
            pressed_button_time = 0;

            if ( setup_minus ) {
                setup_realpart--;

                if ( setup_realpart < 0 ) {
                    setup_intpart++;
                    if ( setup_intpart > 0 ) {
                        setup_intpart = 0;
                        setup_realpart = 0;
                        setup_minus = false;
                    } else {
                        setup_realpart = 9;
                    }
                }

            } else {
                setup_realpart++;
                if ( setup_realpart > 9 ) {
                    setup_realpart = 0;
                    setup_intpart++;
                    if ( setup_intpart > setup_max_value ) {
                        setup_intpart = setup_min_value;
                        setup_minus = true;
                    }
                }
            }


            if ( display_state != Setup ) {
                old_display_state = display_state;
                display_state = Setup;
            }

            PORTB |= ( 1 << PIN_LED );
        } else {
            PORTB &= ~( 1 << PIN_LED );
        }

        // 50*100 ms = 5 seconds
        // so long the Setup mode is shown unless time expires
        if (display_state == Setup) {
            pressed_button_time++;
            if (pressed_button_time > 100) {
                display_state = old_display_state; 
            }
        }


        if ( RCTSwitch_available() ) {

            data = RCTSwitch_getValue();
            RCTSwitch_reset();
            if ( sender_id == sender_id ) {
                uint8_t recv_sender_id = unpack_senderId(data);
                uint8_t recv_intpart = unpack_intpart(data);
                uint8_t recv_realpart = unpack_realpart(data);
                bool recv_is_minus = unpack_minus(data);
     
		// blink only once if correct data received
                led_on(); _delay_ms(50); led_off();
            } else {
                // blinking three times when data 
                // for incorrect sender_id is caught
                led_on(); _delay_ms(50); led_off();
                led_on(); _delay_ms(50); led_off();
                led_on(); _delay_ms(50); led_off();
            }
        }

        /* and finally display what is selected at the moment */
        if (display_state == Temp) {
            display_temp(data);
        } else if (display_state == Setup) {
            display_temp(packtemp( abs(setup_intpart), setup_realpart, setup_minus ));
        }

    }
}

void main(void) {
    setup();
    asm("sei");
    for (;;) {
        loop();
    }
}

