#include <avr/io.h>
#include <avr/interrupt.h>
#include "common.h"

unsigned long ovrf = 0;

ISR(TIM0_OVF_vect) {
	ovrf++;
}


void attiny_init() {
        //Setup timer interrupt and PWM pins
        TCCR0B |= _BV(CS00);
        TCCR0A |= _BV(WGM00)|_BV(WGM01);
#if __AVR__ == __AVR_ATtiny13a__ || __AVR__ == __AVR_ATtiny13__ || __AVR_ATtiny85__
        TIMSK |= 2;
#else
        TIMSK0 |= 2;
#endif
        TCNT0=0;
        sei();
}

unsigned long millis() {
        unsigned long x;
        asm("cli");
        /*Scale number of timer overflows to milliseconds*/
        #if F_CPU < 150000 && F_CPU > 80000
        x = ovrf * 2;
    #elif F_CPU == 600000
        x = ovrf / 2;
        #elif F_CPU == 1000000
        x = ovrf / 4;
        #elif F_CPU == 1200000
        x = ovrf / 5;
        #elif F_CPU == 4000000
        x = ovrf / 16;
        #elif F_CPU == 4800000
        x = ovrf / 19;
        #elif F_CPU == 8000000
        x = ovrf / 31;
        #elif F_CPU == 9600000
        x = ovrf / 37;
    #elif F_CPU == 10000000
        x = ovrf / 39;
        #elif F_CPU == 12000000
        x = ovrf / 47;
        #elif F_CPU == 16000000
        x = ovrf / 63;
        #else
        #error This CPU frequency is not defined
        #endif
        asm("sei");
        return x;
}

unsigned long micros(){
        unsigned long x;
        asm("cli");
        #if F_CPU < 150000 && F_CPU > 80000
        x = ovrf * 2000;
        #elif F_CPU == 600000
        x = ovrf * 427;
        #elif F_CPU == 1000000
        x = ovrf * 256;
        #elif F_CPU == 1200000
        x = ovrf * 213;
        #elif F_CPU == 4000000
        x = ovrf * 64;
        #elif F_CPU == 4800000
        x = ovrf * 53;
        #elif F_CPU == 8000000
        x = ovrf * 32;
        #elif F_CPU == 9600000
        x = ovrf * 27;
        #elif F_CPU == 10000000
        x = ovrf * 26;
        #elif F_CPU == 12000000
        x = ovrf * 21;
        #elif F_CPU == 16000000
        x = ovrf * 16;
        #else
        #error This CPU frequency is not defined
        #endif
        asm("sei");
        return x;
}


void digitalWrite(uint8_t pin, uint8_t value) {
    if (value == HIGH)
        PORTB |= _BV(pin);
    else
	PORTB &= ~_BV(pin);
}

void pinMode(uint8_t pin, uint8_t mode) {
    if (mode == OUTPUT)
	DDRB |= (1 << pin);
    else
	DDRB &= ~(1 << pin);
}

/* Delay for the given number of microseconds.  Assumes a 1, 8, 12, 16, 20 or 24 MHz clock. */
void delayMicroseconds(unsigned int us)
{
        // call = 4 cycles + 2 to 4 cycles to init us(2 for constant delay, 4 for variable)

        // calling avrlib's delay_us() function with low values (e.g. 1 or
// 2 microseconds) gives delays longer than desired.
 //delay_us(us);
#if F_CPU >= 24000000L
 // for the 24 MHz clock for the aventurous ones, trying to overclock

 // zero delay fix
 if (!us) return; //  = 3 cycles, (4 when true)

 // the following loop takes a 1/6 of a microsecond (4 cycles)
 // per iteration, so execute it six times for each microsecond of
 // delay requested.
 us *= 6; // x6 us, = 7 cycles

 // account for the time taken in the preceeding commands.
 // we just burned 22 (24) cycles above, remove 5, (5*4=20)
 // us is at least 6 so we can substract 5
 us -= 5; //=2 cycles

#elif F_CPU >= 20000000L
 // for the 20 MHz clock on rare Arduino boards

// for a one-microsecond delay, simply return.  the overhead
   // of the function call takes 18 (20) cycles, which is 1us
   __asm__ __volatile__ (
           "nop" "\n\t"
           "nop" "\n\t"
           "nop" "\n\t"
           "nop"); //just waiting 4 cycles
   if (us <= 1) return; //  = 3 cycles, (4 when true)

   // the following loop takes a 1/5 of a microsecond (4 cycles)
   // per iteration, so execute it five times for each microsecond of
   // delay requested.
   us = (us << 2) + us; // x5 us, = 7 cycles

   // account for the time taken in the preceeding commands.
   // we just burned 26 (28) cycles above, remove 7, (7*4=28)
   // us is at least 10 so we can substract 7
   us -= 7; // 2 cycles

#elif F_CPU >= 16000000L

// for the 16 MHz clock on most Arduino boards

// for a one-microsecond delay, simply return.  the overhead
// of the function call takes 14 (16) cycles, which is 1us
if (us <= 1) return; //  = 3 cycles, (4 when true)

// the following loop takes 1/4 of a microsecond (4 cycles)
// per iteration, so execute it four times for each microsecond of
// delay requested.
us <<= 2; // x4 us, = 4 cycles

// account for the time taken in the preceeding commands.
// we just burned 19 (21) cycles above, remove 5, (5*4=20)
// us is at least 8 so we can substract 5
us -= 5; // = 2 cycles,

#elif F_CPU >= 12000000L
// for the 12 MHz clock if somebody is working with USB

// for a 1 microsecond delay, simply return.  the overhead
// of the function call takes 14 (16) cycles, which is 1.5us
if (us <= 1) return; //  = 3 cycles, (4 when true)

// the following loop takes 1/3 of a microsecond (4 cycles)
// per iteration, so execute it three times for each microsecond of
// delay requested.
us = (us << 1) + us; // x3 us, = 5 cycles

// account for the time taken in the preceeding commands.
// we just burned 20 (22) cycles above, remove 5, (5*4=20)
// us is at least 6 so we can substract 5
us -= 5; //2 cycles


#elif F_CPU >= 8000000L
        // for the 8 MHz internal clock

        // for a 1 and 2 microsecond delay, simply return.  the overhead
        // of the function call takes 14 (16) cycles, which is 2us
        if (us <= 2) return; //  = 3 cycles, (4 when true)

        // the following loop takes 1/2 of a microsecond (4 cycles)
        // per iteration, so execute it twice for each microsecond of
        // delay requested.
        us <<= 1; //x2 us, = 2 cycles

        // account for the time taken in the preceeding commands.
        // we just burned 17 (19) cycles above, remove 4, (4*4=16)
        // us is at least 6 so we can substract 4
        us -= 4; // = 2 cycles

#else
        // for the 1 MHz internal clock (default settings for common Atmega microcontrollers)

// the overhead of the function calls is 14 (16) cycles
if (us <= 16) return; //= 3 cycles, (4 when true)
if (us <= 25) return; //= 3 cycles, (4 when true), (must be at least 25 if we want to substract 22)

// compensate for the time taken by the preceeding and next commands (about 22 cycles)
us -= 22; // = 2 cycles
// the following loop takes 4 microseconds (4 cycles)
// per iteration, so execute it us/4 times
// us is at least 4, divided by 4 gives us 1 (no zero delay bug)
us >>= 2; // us div 4, = 4 cycles


#endif

// busy wait
__asm__ __volatile__ (
        "1: sbiw %0,1" "\n\t" // 2 cycles
        "brne 1b" : "=w" (us) : "0" (us) // 2 cycles
);
// return = 4 cycles
}




