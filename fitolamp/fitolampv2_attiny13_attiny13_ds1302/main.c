/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>


             ATtiny13
            +--------+
 RESET <--- +        + ---> +5V
SENSOR <--- +        + ---> RTC CLK
 RELAY <--- +        + ---> RTC IO
   GND <--- +        + ---> RTC CE
            +--------+

Features:
- works with DS1302 RTC
- checks if the current time is the operational time
- goes into the power down mode for 8 seconds
- takes into account the current lighting
- manages a relay (PBB3/LED)
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "ds1302.h"

#define PIN_CE  PB0 // rst
#define PIN_IO  PB1 // dat
#define PIN_CLK PB2 // clk
#define PIN_ADC PB3
#define PIN_LED PB4

#define OPER_HOURS_FROM    05
#define OPER_MINUTES_FROM  30
#define OPER_HOURS_TO      22
#define OPER_MINUTES_TO    30


int main( void ) {
    DS1302Tiny_init(PIN_CE, PIN_IO, PIN_CLK);
    DDRB |= ( 1 << PIN_LED );
    PORTB &= ~( 1 << PIN_LED );

    // adc_setup
    ADMUX |= ( 1 << MUX0 ) | ( 1 << MUX1 ) | ( 1 << ADLAR );  //ADC3/PB3
    ADCSRA |= ( 1 << ADPS2 ) | ( 1 << ADEN ); 
 
    wdt_reset();
    wdt_enable( WDTO_8S );
    WDTCR |= _BV( WDTIE ); // enabing watchdog timer
    sei();
    set_sleep_mode( SLEEP_MODE_PWR_DOWN );
    sleep_enable();
    sleep_cpu();
    while ( 1 ) {};
    return 0;
}
/*
uint8_t read_adc() {
    ADCSRA |= ( 1 << ADSC );
    while ( ADCSRA & ( 1 << ADSC ));
    return ADCH;
}
*/
ISR( WDT_vect ) {
//void loop_ex( void ) {
    ShortTime_t tm;
    if ( DS1302Tiny_getTime( &tm )) {
        if (((( tm.hour == OPER_HOURS_FROM ) && ( tm.minute >= OPER_MINUTES_FROM )) || ( tm.hour > OPER_HOURS_FROM )) && 
             (( tm.hour < OPER_HOURS_TO ) || ( tm.hour == OPER_HOURS_TO && tm.minute <= OPER_MINUTES_TO ))) {

            // read ADC 
            ADCSRA |= ( 1 << ADSC );
            while ( ADCSRA & ( 1 << ADSC ));

            if ( ADCH < 0x7F ) {
                PORTB |= ( 1 << PIN_LED );
            } else {
                PORTB &= ~( 1 << PIN_LED );
            }
        }
    }
    WDTCR |= _BV( WDTIE );
}

