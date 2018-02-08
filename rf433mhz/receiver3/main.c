#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//#include <avr/power.h>
#include "tm1637.h" // Pins CLK and DIO are predefined, see the tm1637.h file
//#include "rctswitch.h"
#include "common.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define mem_address 0x11

// uint16_t RCTSwitch_getValue();
// void RCTSwitch_setup();
// uint8_t RCTSwitch_available();

#define PIN_LED PB1
// #define PIN_PB0 PB0 // PCINT0 vector uses it
#define PIN_BUTTON PB2
// PB0 is the PCINT0 interrupt is allocated to RF433 receiver
// PB3 & PB4 occupied by TM1637 display
// #define PIN_TRANSMITTER PB4
#define RCSWITCH_MAX_CHANGES 67
#define nReceiveTolerance 60

#define led_on()     sbi(PORTB, PIN_LED)
#define led_off()    cbi(PORTB, PIN_LED)


#define unpack_senderId(x)  	( 0x7 & ( x >> 11 ))
#define unpack_intpart(x) 	( 0x3F & x )
#define unpack_realpart(x)      ( 0xF & ( x >> 6 ))
#define unpack_minus(x)	        (( 0x1 & ( x >> 10 )) > 0 ) ? true : false

#define MAX_DEBUG 10
int debug_id = 0;
static uint16_t debug[MAX_DEBUG];

//void add2debug(uint16_t value) { debug[debug_id++] = value; };
void add2debug(const int index, uint16_t value) { debug[index] = value; };
uint16_t getdebug(const int index) { return debug[index]; };




static bool has_interrupt = false;
static uint16_t data = 0;	// it stores received data
//static uint16_t debug_num = 0;
volatile int display_number(const uint16_t value);

uint16_t packtemp(int8_t intpart, int8_t realpart, bool is_minus) {
    return ( intpart ) | ( realpart << 6 ) | ( is_minus << 10 );
}

struct pulse_t {
    uint8_t high;
    uint8_t low;
};

struct protocol_t {
    int pulse_length;
    struct pulse_t sync_factor;
    struct pulse_t zero;
    struct pulse_t one;
    bool inverted;
};


const struct protocol_t proto = { 350, { 1, 31 }, { 1, 3 }, { 3, 1 }, false };
volatile unsigned int timings[RCSWITCH_MAX_CHANGES];
volatile uint16_t nReceivedValue = 0;
volatile uint8_t nReceivedDelay = 0;

int abs(int __i);

volatile inline int diff(int A, int B) { return abs(A - B); }
inline uint16_t RCTSwitch_getValue() { return nReceivedValue; }
volatile inline bool RCTSwitch_available() { return nReceivedValue != 0; }
inline void RCTSwitch_reset() { nReceivedValue = 0; }

bool RCTSwitch_receiveProtocol(const int p, const int changeCount) {
    const unsigned int syncLegthInPulses = ((proto.sync_factor.low) > (proto.sync_factor.high)) ? proto.sync_factor.low : proto.sync_factor.high;
    const uint16_t delay_ms = timings[0] / syncLegthInPulses;
    const uint16_t delayTolerance = ( delay_ms * nReceiveTolerance ) / 100;


    uint16_t code = 0;
    const uint16_t firstDataTiming = proto.inverted ? (2) : (1);
    uint16_t i;
    for ( i = firstDataTiming; i < changeCount - 1; i += 2 ) {
        code <<= 1;
        if (diff(timings[i], delay_ms*proto.zero.high) < delayTolerance &&
            diff(timings[i+1], delay_ms*proto.zero.low) < delayTolerance) {
            // zero
            if ( ! has_interrupt ) 
                add2debug( 1, getdebug(1)+1 );
        } else if (diff(timings[i], delay_ms*proto.one.high) < delayTolerance &&
                   diff(timings[i+1], delay_ms*proto.one.low) < delayTolerance) {
            code |= 1;
            if ( ! has_interrupt ) 
                add2debug( 2, getdebug(2)+1 );
        } else {
            // failed
            if ( ! has_interrupt )
                add2debug( 3, getdebug(3)+1 );
            return false;
        }
    }
    if ( changeCount > 15 ) {
        //code &= ~( 1 << 7 );
        nReceivedValue = code;
        nReceivedDelay = delay_ms; 
        if ( ! has_interrupt )
            add2debug( 4, code );
        return true;
    }
    return false;
}

ISR(PCINT0_vect) {
    led_on();
    static unsigned int changeCount = 0;
    static unsigned long lastTime = 0;
    static unsigned int repeatCount = 0;
    const long time = micros();
    const unsigned int duration = time - lastTime;
    if ( duration > 4300 ) {
        add2debug( 6, getdebug(6)+1 );
        if ( diff( duration, timings[0] ) < 200 ) {
            repeatCount++;
            if ( repeatCount == 2 ) {
                RCTSwitch_receiveProtocol(1, changeCount );
                repeatCount = 0;
            }
        }
        changeCount = 0;
    } else {
        if ( getdebug(5) == 0 ) {
            add2debug( 5, duration );
        }
    }

    if ( changeCount >= RCSWITCH_MAX_CHANGES ) {
        changeCount = 0;
        repeatCount = 0;
        //debug_num = 3;
    }
    timings[changeCount++] = duration;
    lastTime = time;
    led_off();
    has_interrupt = true;
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

volatile int display_number(const uint16_t value) {
  int d0 = value / 1000;
  int d1 = ( value - d0*1000 ) / 100;
  int d2 = ( value - d0*1000 - d1*100 ) / 10;
  int d3 = ( value - d0*1000 - d1*100 - d2*10 );
  TM1637_display_digit(TM1637_SET_ADR_00H, d0);
  TM1637_display_digit(TM1637_SET_ADR_01H, d1);
  TM1637_display_digit(TM1637_SET_ADR_02H, d2);
  TM1637_display_digit(TM1637_SET_ADR_03H, d3);
  return 0;
}

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

/*
bool check_controlsum(uint16_t value) {
    uint16_t n = value;
    uint8_t c = 0;
    for ( c = 0; n; n = n & ( n-1 ))
        c++;
    // if number of bits is even
    return ( c % 2 == 0 ) ? true : false;
}
*/

void EEPROM_write(uint8_t ucAddress, int8_t ucData) {
    while ( EECR & ( 1 << EEPE ));
    EECR = ( 0 << EEPM1 ) | ( 0 << EEPM0 );
    EEAR = ucAddress;
    EEDR = ucData;
    cli();
    EECR |= ( 1 << EEMPE );
    EECR |= ( 1 << EEPE );
    sei();
}

int8_t EEPROM_read(unsigned char ucAddress) {
    while ( EECR & ( 1 << EEPE ));
    EEAR = ucAddress;
    EECR |= ( 1 << EERE );
    return EEDR;
}


uint8_t sender_id = 0x5;


enum DisplayState { Temp = 1, Setup = 2 };
enum DisplayState  display_state = Temp;
enum DisplayState old_display_state;

unsigned long pressed_button_time = 0;

int8_t setup_value = 0;
int8_t setup_min_value = -30;
int8_t setup_max_value = 40;
bool colon_blink_state = false;
bool has_threshold_crossed = false;


int8_t setup_intpart = -10;
bool   setup_minus = false;
uint8_t packet_seq = 0;

void loop(void) {

    if ( display_state == Setup && millis() % 200 == 0 ) {
        TM1637_display_colon( ! colon_blink_state );
        colon_blink_state = ! colon_blink_state;
    }


    if (millis() % 100 == 0) {

        // if we have pressed a button
        if ((( 1 << PIN_BUTTON ) & PINB ) > 0 ) {
            _delay_ms(50); // to skip multiple signals
            pressed_button_time = 0;
            setup_intpart++;
            if ( setup_intpart > setup_max_value ) {
                setup_intpart = setup_min_value;
            }
            setup_minus = ( setup_intpart < 0 );

            if ( display_state != Setup ) {
                old_display_state = display_state;
                display_state = Setup;
                setup_intpart = EEPROM_read(mem_address);
            }
        }

        // 50*100 ms = 5 seconds
        // so long the Setup mode is shown unless time expires
        if (display_state == Setup) {
            pressed_button_time++;
            if (pressed_button_time > 50) {
                display_state = old_display_state;
                EEPROM_write(mem_address, setup_intpart);
            }
        }


        //if ( RCTSwitch_available() ) {
        if ( nReceivedValue != 0 ) {
            led_on();
            uint16_t recv_data = RCTSwitch_getValue();
            RCTSwitch_reset();

            // uint16_t value = unpack_intpart(recv_data);

            if ( data != recv_data ) {
                led_on(); _delay_ms(500); led_off();
                data = recv_data;

                //if ( 1 ) {
                if ( sender_id == unpack_senderId(recv_data) ) {
                    //uint8_t recv_sender_id = unpack_senderId(recv_data); 
                    /*
                    uint8_t recv_intpart = unpack_intpart(recv_data);
                    uint8_t recv_packet_seq = unpack_realpart(recv_data);

                    if ( recv_packet_seq != packet_seq ) {
                        packet_seq = recv_packet_seq;
                    };

                    uint8_t t_value = setup_value;
                    */
                    /*
                    if ( setup_minus )
                        t_value *= -1;
                    // has_threshold_crossed = false;
                    */
                    // to display it when needed
                    /*
                    //bool recv_is_minus = unpack_minus(recv_data);
                    if ( check_controlsum(recv_data) ) {
    		    // blink only once if correct data received
                        if ( ! has_threshold_crossed ) {
                            led_on(); _delay_ms(50); led_off();
                        }
                    } else {
    		    // blink two times if a check sum is incorrect
                        if ( ! has_threshold_crossed ) {
                            led_on(); _delay_ms(50); led_off();
                            led_on(); _delay_ms(50); led_off();
                            led_on(); _delay_ms(50); led_off();
                        }
                    }
                    */
                }
            } else {
                // blinking three times when data
                // for incorrect sender_id is caught
                if ( ! has_threshold_crossed ) {
                    led_on(); _delay_ms(50); led_off();
                    led_on(); _delay_ms(50); led_off();
                    led_on(); _delay_ms(50); led_off();
                }
            }
        }

        /* and finally display what is selected at the moment */
        if (display_state == Temp) {
            //display_temp(data);
            display_number(data);
        } else if (display_state == Setup) {
            display_temp(packtemp( abs(setup_intpart), 0, setup_minus ));
        }
    }
}

void RCTSwitch_setup() {
    GIMSK |= ( 1 << PCIE ); //|(1<<INT0);
    PCMSK |= ( 1 << PCINT0 );
    MCUCR |= ( 1 << ISC00 );
}


void setup(void) {
    attiny_init();
    TM1637_init();
    RCTSwitch_setup();
    DDRB |= (1 << PIN_LED);
    DDRB &= ~( 1 << PIN_BUTTON );
    PORTB &= ~( 1 << PIN_BUTTON );
    setup_intpart = EEPROM_read(mem_address);
    setup_minus = ( setup_intpart < 0 ) ? true : false;
}

int main(void) {
    setup();
    asm("sei");

    for (;;) {
        loop2();
    }
    return 0;
}

void loop2(void) {
    if ( has_interrupt ) {
        debug_id = 8;
        while ( debug_id > 0 ) { 
            display_number(--debug_id);
            _delay_ms(750);
            display_number(debug[debug_id]);
            _delay_ms(1400);
        }
        add2debug( 6, 0 );
        add2debug( 5, 0 );
        has_interrupt = false;
        debug_id = 0;
    }
}
