/****

Sources is taken from here http://forum.sources.ru/index.php?showtopic=381077

****/

#include <inttypes.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
 
// --- Configuration for OneWire
#define ONEWIRE_PORTReg     PORTB
#define ONEWIRE_DDRReg      DDRB
#define ONEWIRE_PINReg      PINB
#define ONEWIRE_PIN         PB0


#define TX_PORTReg          PORTB
#define TX_DDRReg           DDRB
#define TX_PINReg           PINB
#define TX_PIN              PB2

#define pulse_len 350
 
// --- Configure for PWM
//#define PWM_PORTReg         PORTB
//#define PWM_DDRReg          DDRB
//#define PWM_PIN             PB1

// - Set OneWire pin in Output mode
#define OneWire_setPinAsOutput  ONEWIRE_DDRReg |= (1<<ONEWIRE_PIN)
 
// - Set OneWire pin in Input mode
#define OneWire_setPinAsInput   ONEWIRE_DDRReg &= ~(1<<ONEWIRE_PIN)
 
// - Set LOW level on OneWire pin
#define OneWire_writePinLOW     ONEWIRE_PORTReg &=  ~(1<<ONEWIRE_PIN)
 
// - Set HIGH level on OneWire pin
#define OneWire_writePinHIGH    ONEWIRE_PORTReg |= (1<<ONEWIRE_PIN)
 
// - Read level from OneWire pin
#define OneWire_readPin         ( ( ONEWIRE_PINReg & (1<<ONEWIRE_PIN) ) ? 1 : 0 )


#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif 
 
/* 
// - PWM Value ( 0 - off, 255 - max )
uint8_t PWMValue    = 0;
 
// - Oveflow counter for PWM
uint8_t PWMCounter  = 0;
uint8_t PWM         = 0;
*/


static uint8_t data; 
 
//! Calculate CRC-8
uint8_t crc8(const uint8_t * addr, uint8_t len){
    uint8_t crc = 0;
    while (len--) {
        uint8_t inbyte = *addr++;
        uint8_t i;
        for (i = 8; i; i--) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
}
 
//! Reset function
uint8_t OneWire_reset(){
    
    // - Wait for line
    uint8_t Retries = 125;
    OneWire_setPinAsInput;
    do{
        if( --Retries == 0 ) return 0;
        _delay_us( 2 );
    }while( !OneWire_readPin );
    
    // - Drop line
    OneWire_writePinLOW;
    OneWire_setPinAsOutput;
    _delay_us( 480 );
    
    // - Listen for reply pulse
    OneWire_setPinAsInput;
    _delay_us( 70 );
    
    // - Read line state
    uint8_t State = !OneWire_readPin;
    _delay_us( 410 );
    return State;
}
 
//! Write single bit
void OneWire_writeBit( uint8_t Bit ){
    if( Bit & 1 ){
        // - Drop line
        OneWire_writePinLOW;
        OneWire_setPinAsOutput;
        // - Write Bit-1
        _delay_us( 10 );
        OneWire_writePinHIGH;
        _delay_us( 55 );
    }else{
        // - Drop line
        OneWire_writePinLOW;
        OneWire_setPinAsOutput;
        // - Write Bit-0
        _delay_us( 65 );
        OneWire_writePinHIGH;
        _delay_us( 5 );
    }
}
 
//! Read single bit
uint8_t OneWire_readBit(){
    // - Drop line
    OneWire_setPinAsOutput;
    OneWire_writePinLOW;
    _delay_us( 3 );
    
    // - Wait for data
    OneWire_setPinAsInput;
    _delay_us( 10 );
    
    // - Read bit into byte
    uint8_t Bit = OneWire_readPin;
    _delay_us( 53 );
    return Bit;
}
 
//! Write byte
void OneWire_writeByte( const uint8_t Byte, uint8_t Power ){
    
    // - Write each bit
    uint8_t BitMask;
    for( BitMask = 0x01; BitMask; BitMask <<= 1 ) OneWire_writeBit( (BitMask & Byte) ? 1 : 0 );
    
    // - Disable power
    if( !Power ){
        
        OneWire_setPinAsInput;
        OneWire_writePinLOW;
    }
}
//! Read byte
inline uint8_t OneWire_readByte(){
    uint8_t Byte = 0;
    
    // - Read all bits
    uint8_t BitMask;
    for( BitMask = 0x01; BitMask; BitMask <<= 1 ){
        // - Read & store bit into byte
        if( OneWire_readBit() ) Byte |= BitMask;
    }
    return Byte;
}
 
 
//! Read buffer
inline void OneWire_read( uint8_t * Buffer, uint8_t Size ){
    uint8_t i; 
    for( i = 0; i < Size; i++ ) Buffer[ i ] = OneWire_readByte();
}
 
//! Write buffer
inline void OneWire_write(const uint8_t * Buffer, uint8_t Size, uint8_t Power ){
    uint8_t i;
    for( i = 0; i < Size; i++ )  OneWire_writeByte(  Buffer[ i ], Power );
    if( !Power ){
        // - Disable power
        OneWire_setPinAsInput;
        OneWire_writePinLOW;
    }
}


void sendbyte(const int pin) {
    uint8_t k = 10;
    int i;
    while (k-- > 0) {
        for (i = sizeof(data)*8 - 1; i >= 0; i--) {
            sbi(PORTB, pin);
            if ( data & ( 1 << i )) {
                _delay_us(3 * pulse_len);
                cbi(PORTB, pin);
                _delay_us(1 * pulse_len);
            } else {
                _delay_us(1 * pulse_len);
                cbi(PORTB, pin);
                _delay_us(3 * pulse_len);
            }
        }
        sbi(PORTB, pin);
        _delay_us(1 * pulse_len);
        cbi(PORTB, pin);
        _delay_us(31 * pulse_len);
    }
}
 
 
//! Main function
int main() {
 
    // - Configure LED pin
    DDRB |= ( 1 << PB1 ) | ( 1 << TX_PIN ) | ( 1 << PB3 ) ; 
    
    
    // - Buffer for ROM
    uint8_t ROM[ 9 ];
    while( 1 ){
        
        // - Prepare for new cycle
        _delay_ms( 1000 );
        memset( ROM, 0, sizeof( ROM ) );
        
        // - Start conversion
        OneWire_reset();
        OneWire_writeByte( 0xCC, 1 );
        OneWire_writeByte( 0x44, 1 );
        
        // - Wait until conversion finished
        _delay_ms( 1000 );
        
        // - Read ROM
        OneWire_reset();
        OneWire_writeByte( 0xCC, 1 );
        OneWire_writeByte( 0xBE, 1 );
        OneWire_read( ROM, sizeof( ROM ) );
        
        // - Check ROM CRC
        if( crc8( ROM, 8 ) != ROM[ 8 ] ){
            continue;
        }
        
        // --- Get 8-bit temperature
        // - Construct 16-bit register value from 0 and 1 bytes of ROM.
        // - Remove float part (4 right bits) to get interger value
        uint8_t     Temperature     =  ((( ROM[ 1 ] << 8 ) | ROM[ 0 ]) >> 4);
        
        
        if( Temperature > 30 ){
            PORTB |= ( 1 << PB1 );
        } else {
            PORTB &= ~( 1 << PB1 );
        }

        data = Temperature;
        PORTB |= ( 1 << PB3 );
        sendbyte(TX_PIN);
        _delay_ms(200);
        PORTB &= ~( 1 << PB3 );
        _delay_ms(900);
    }
}
 
