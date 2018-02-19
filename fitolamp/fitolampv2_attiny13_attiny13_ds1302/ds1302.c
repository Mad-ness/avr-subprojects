#include <avr/io.h>
#include <util/delay.h>
#include "ds1302.h"
/*
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1U << (bit)))
#define bitClear(value, bit) ((value) &= ~(1U << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#define sbi( reg, bit ) ( reg |= ( 1 << bit ))
#define cbi( reg, bit ) ( reg &= ~( 1 << bit ))
*/
//void bitWrite( uint8_t value, const int bit, const int value) {
//    (bitvalue ? bitSet(value, bit) : bitClear(value, bit));
//}

#define DS1302_SECONDS           0x80
#define DS1302_MINUTES           0x82
#define DS1302_HOURS             0x84
#define DS1302_DATE              0x86
#define DS1302_MONTH             0x88
#define DS1302_DAY               0x8A
#define DS1302_YEAR              0x8C
#define DS1302_ENABLE            0x8E
#define DS1302_TRICKLE           0x90
#define DS1302_CLOCK_BURST       0xBE
#define DS1302_CLOCK_BURST_WRITE 0xBE
#define DS1302_CLOCK_BURST_READ  0xBF
#define DS1302_RAM_START         0xC0
#define DS1302_RAM_END           0xFC
#define DS1302_RAM_BURST         0xFE
#define DS1302_RAM_BURST_WRITE   0xFE
#define DS1302_RAM_BURST_READ    0xFF



// Defines for the bits, to be able to change
// between bit number and binary definition.
// By using the bit number, using the DS1302
// is like programming an AVR microcontroller.
// But instead of using "(1<<X)", or "_BV(X)",
// the Arduino "bit(X)" is used.
#define DS1302_D0 0
#define DS1302_D1 1
#define DS1302_D2 2
#define DS1302_D3 3
#define DS1302_D4 4
#define DS1302_D5 5
#define DS1302_D6 6
#define DS1302_D7 7


// Bit for reading (bit in address)
#define DS1302_READ     DS1302_D0 // READ=1, WRITE=0

// Bit for clock (0) or ram (1) area,
// called R/C-bit (bit in address)
#define DS1302_RC       DS1302_D6

// In Seconds Register
#define DS1302_CH       DS1302_D7 // 1 = Clock Halt, 0 = start

// In Hours Register
#define DS1302_AM_PM    DS1302_D5 // 0 = AM, 1 = PM
#define DS1302_12_24    DS1302_D7 // 0 = 24 hour, 1 = 12 hour

// Enable Write Protect Register
#define DS1302_WP       DS1302_D7 // 1 = Write Protect, 0 = enabled

// Trickle Register
#define DS1302_ROUT0    DS1302_D0 // Resistor 2kR (1 for 8kR)
#define DS1302_ROUT1    DS1302_D1 // Resistor 4kR (1 for 8kR)
#define DS1302_DS0      DS1302_D2 // 1 Diode
#define DS1302_DS1      DS1302_D3 // 2 Diodes
#define DS1302_TCS0     DS1302_D4 // Trickle-charge select (0 for enable)
#define DS1302_TCS1     DS1302_D5 // Trickle-charge select (1 for enable)
#define DS1302_TCS2     DS1302_D6 // Trickle-charge select (0 for enable)
#define DS1302_TCS3     DS1302_D7 // Trickle-charge select (1 for enable)

#define DS1302_TCR_INI  B01011100 // Initial power-on state (disabled)
#define DS1302_TCR_2D8K B10101011 // 2 Diodes, 8kΩ
#define DS1302_TCR_2D4K B10101010 // 2 Diodes, 4kΩ
#define DS1302_TCR_2D2K B10101001 // 2 Diodes, 2kΩ
#define DS1302_TCR_1D8K B10100111 // 1 Diodes, 8kΩ
#define DS1302_TCR_1D4K B10100110 // 1 Diodes, 4kΩ
#define DS1302_TCR_1D2K B10100101 // 1 Diodes, 2kΩ

// volatile int ce_pin, io_pin, sclk_pin;
static int ce_pin, io_pin, sclk_pin;

void toggleWrite( uint8_t value );
uint8_t toggleRead( void );
void toggleStop( void );
void toggleStart( void );
uint8_t bcd2dec( uint8_t num );

//void DS1302Tiny_writeRTCi( uint8_t *p );
//void DS1302Tiny_writeRTC( uint8_t address, uint8_t value );

bool DS1302Tiny_getTime( ShortTime_t *tm ) {
    uint8_t buff[8];
    DS1302Tiny_readRTC( buff );
    tm->hour = bcd2dec( buff[2] & 0x3F );
    tm->minute = bcd2dec( buff[1] & 0x7F );
    tm->second = bcd2dec( buff[0] & 0x7F );
    if (( tm->minute >= 0 && tm->minute <= 59 ) && ( tm->hour >=0 && tm->hour <= 23 ) && 
        ( tm->second >= 0 && tm->second <= 59 )) {
        return true;
    } else {
        return false;
    }
};

uint8_t bcd2dec( uint8_t num ) {
    return (( num/16 * 10 ) + ( num % 16 ));
};

void DS1302Tiny_init( const int ce, const int io, const int sclk ) {
    ce_pin = ce;
    io_pin = io;
    sclk_pin = sclk;
}

void DS1302Tiny_readRTC( uint8_t *p ) {
    toggleStart();
    toggleWrite( DS1302_CLOCK_BURST_READ );
    int i;
    for ( i = 0; i < 8; i++ ) {
        *p++ = toggleRead();
    }
    toggleStop();
};

void toggleWrite( uint8_t value ) {
    int i;
    for ( i = 0; i <= 7; i++ ) {
        bitRead( value, i ) == 1 ? sbi( PORTB, io_pin ) : cbi( PORTB, io_pin );
        _delay_us( 1 );
        sbi( PORTB, sclk_pin );
        _delay_us( 1 );
        cbi( PORTB, sclk_pin );
        _delay_us( 1 );
    }
}

uint8_t toggleRead( void ) {
    int i = 0;
    uint8_t value = 0;
    DDRB &= ~( 1 << io_pin );
    for ( i = 0; i <= 7; i++ ) {
        bitWrite( value, i, bitRead( PINB, io_pin ));
        sbi( PORTB, sclk_pin );
        _delay_us( 1 );
        cbi( PORTB, sclk_pin );
        _delay_us( 1 );
    }
    return value;
}

void toggleStop( void ) {
    PORTB &= ~( 1 << ce_pin );
    _delay_us( 4 );
}

void toggleStart( void ) {
    PORTB &= ~( 1 << ce_pin );
    DDRB |= ( 1 << ce_pin );

    PORTB &= ~( 1 << sclk_pin );
    DDRB |= ( 1 << sclk_pin );

    DDRB |= ( 1 << io_pin );

    PORTB |= ( 1 << ce_pin );
    _delay_us( 4 );
}


