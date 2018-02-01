#include <avr/io.h>
#include <avr/interrupt.h>
#include "rctswitch.h"


const struct protocol_t proto = { 350, {1, 31}, {1, 3}, {3, 1}, false };
#if defined RCTSWITCH_RECEIVER

#define nReceiveTolerance 	60
#define RCSWITCH_MAX_CHANGES	67
#define LISTEN_PIN              PB0

static unsigned int timings[RCSWITCH_MAX_CHANGES];
volatile uint16_t nReceivedValue = 0;
volatile uint8_t nReceivedDelay = 0;
static int nValueAvalable       = false;


ISR(PCINT0_vect) {
  RCTSwitch_interruptHandler();
}

static inline unsigned long diff(int A, int B) {
  return abs(A - B);
}

#if defined RCTSWITCH_RECEIVER
void RCTSwitch_setup() {
    GIMSK |= ( 1 << PCIE ); //|(1<<INT0);
    PCMSK |= ( 1 << PCINT0 );
    MCUCR |= ( 1 << ISC00 );
    asm("sei");
}
#endif // RCTSWITCH_RECEIVER


uint8_t RCTSwitch_available() {
    return nValueAvalable;
}

uint16_t RCTSwitch_getValue() {
    return nReceivedValue;
}

void RCTSwitch_reset() {  
    nReceivedValue = 0;
    nValueAvalable = false;
}

void RCTSwitch_receiveProtocol(const int p, const int changeCount) {
  const unsigned int syncLegthInPulses = ((proto.sync_factor.low) > (proto.sync_factor.high)) ? proto.sync_factor.low : proto.sync_factor.high;
  const unsigned int delay_ms = timings[0] / syncLegthInPulses;
  const unsigned int delayTolerance = (delay_ms * nReceiveTolerance) / 100;

  unsigned long code = 0;
  const unsigned int firstDataTiming = proto.inverted ? (2) : (1);

  unsigned int i = 0;
  for (i = firstDataTiming; i < changeCount - 1; i += 2) {
    code <<= 1;
    if (diff(timings[i], delay_ms*proto.zero.high) < delayTolerance && 
        diff(timings[i+1], delay_ms*proto.zero.low) < delayTolerance) {
      // zero
    } else if (diff(timings[i], delay_ms*proto.one.high) < delayTolerance && 
               diff(timings[i+1], delay_ms*proto.one.low) < delayTolerance) {
      code |= 1;
    } else {
      // failed
      return;
    }
  }
  if (changeCount > 7) {
    nReceivedValue = code;
    nReceivedDelay = delay_ms;
    nValueAvalable = true;
  } 
}

// Call this function in an ISR
void RCTSwitch_interruptHandler() {
  static unsigned int changeCount = 0;
  static unsigned long lastTime = 0;
  static unsigned int repeatCount = 0;
  const long time = micros();
  const unsigned int duration = time - lastTime;
  
  if (duration > 4300) {

    if (diff(duration, timings[0]) < 200) {
      repeatCount++;
      if (repeatCount == 2) {
        RCTSwitch_receiveProtocol(1, changeCount);
        repeatCount = 0;
      }
    }
    changeCount = 0;
  }

  if (changeCount >= RCSWITCH_MAX_CHANGES) {
    changeCount = 0;
    repeatCount = 0;
  }
  timings[changeCount++] = duration;
  lastTime = time;
}

#endif // RCTSWITCH_RECEIVER

#if defined RCTSWITCH_TRANSMITTER
void RCTSwitch_setup(const uint8_t pin) {
    DDRB |= ( 1 << pin );
}
void RCTSwitch_sendbyte(const uint8_t pin, const uint8_t data, const int attempts) {
    int i = 7, k = 0;
    for (k = 0; k < attempts; k++) {
        for (i = sizeof(data)*8-1; i >= 0; i--) {
            sbi(PORTB, pin);
            if (( data & (1 << i)) == 0) {
                _delay_us( proto.zero.high*proto.pulse_length );
                cbi(PORTB, pin);
                _delay_us( proto.zero.low*proto.pulse_length );
            } else {
                _delay_us( proto.one.high*proto.pulse_length );
                cbi(PORTB, pin);
                _delay_us( proto.one.low*proto.pulse_length );
            }
        }
        sbi(PORTB, pin);
        _delay_us( proto.sync_factor.high*proto.pulse_length );
        cbi(PORTB, pin);
        _delay_us( proto.sync_factor.low*proto.pulse_length );
    } 
}
#endif // RCTSWITCH_TRANSMITTER

