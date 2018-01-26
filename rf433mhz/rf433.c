#include "rf433.h"
#include <avr/io.h>
#include <util/delay.h>


#define  nPulseLength 350

#define nHighPulses_0   (nPulseLength*1)
#define nLowPulses_0    (nPulseLength*3)

#define nHighPulses_1   (nPulseLength*3)
#define nLowPulses_1    (nPulseLength*1)
#define nLowPulses_sync (nPulseLength*31)

//#define PIN_TX		(1<<PB3) // PB3 pin, goes to transmitter data pin
//#define PIN_LED		(1<<PB4) // PB4 pin, for led

char buffer[8];

char* digit2char(const int data) {
    int i;
    for (i = 0; i < 8; i++)
    {
         if ((data >> i) & 0x1)
           buffer[7-i] = '1';
         else
           buffer[7-i] = '0';
    }
    return buffer;
}

void RF433_sendbyte(const int pin, const int num) {
    RF433_send(pin, digit2char(num));
}

void RF433_sendbyte2(const int pin, const int data, const int attempts) {
     int i = 7, k = 0;
     for (k = 0; k < attempts; k++) {
         while (i+1 > 0) {
             if ( data & (1L << i) == 0 ) {
                 PORTB |= (1 << pin);
                 _delay_us(nHighPulses_0);
                 PORTB &= ~(1 << pin);
                 _delay_us(nLowPulses_0); 
             } else {
                 PORTB |= (1 << pin);
                 _delay_us(nHighPulses_1);
                 PORTB &= ~(1 << pin);
                 _delay_us(nLowPulses_1);
             }
             i--;
         } 
         PORTB |= (1 << pin);
         _delay_us(nHighPulses_0);
     } // make the attempts times to send a byte
     PORTB &= ~(1 << pin);
    _delay_us(nLowPulses_sync);
}

void RF433_send(const int pin, char* sCodeWord){

	while (*sCodeWord != '\0') {
  
		PORTB |= (1 << pin); // same as digitalWrite high

		if(*sCodeWord == '0')
		{
			_delay_us(nHighPulses_0);
			PORTB &= ~(1<<pin); // same as digitalWrite low
			_delay_us(nLowPulses_0);
		}else
		{
			_delay_us(nHighPulses_1);
			PORTB &= ~(1<<pin);
			_delay_us(nLowPulses_1);
		} 

		++sCodeWord;
	}

	PORTB |= (1<<pin);
	_delay_us(nHighPulses_0);

	PORTB &= ~(1<<pin);
	_delay_us(nLowPulses_sync);
}


void RF433_setup(const int pin) {         
	DDRB |= (1<<pin); // Set output direction on PIN_TX
}

