#ifndef __COMMON_H__
#define __COMMON_H__

#include <avr/io.h>
#include <inttypes.h>

#define OUTPUT 1
#define INPUT  0
#define HIGH   1
#define LOW    0


#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


void digitalWrite(uint8_t pin, uint8_t value);
void pinMode(uint8_t pin, uint8_t mode);
void delayMicroseconds(unsigned int msecs);
unsigned long millis();
unsigned long micros();
void attiny_init();



#endif // __COMMON_H__

