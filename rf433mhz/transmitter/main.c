#include <avr/io.h>
#include <util/delay.h>
//#include <tm1637.h> // doesn't fit into the attiny13 flash memory
//#include <common.h>
#include <rctswitch.h>

#define PIN_LED_SEND PB0
#define PIN_TRANSMITTER PB4

/*
void display_test(const uint8_t number) {
    int d1 = number / 100;
    int d2 = (number - d1*100)/10;
    int d3 = (number - d1*100 - d2*10);
    TM1637_display_digit(TM1637_SET_ADR_00H, d1);
    TM1637_display_digit(TM1637_SET_ADR_01H, d2);
    TM1637_display_digit(TM1637_SET_ADR_02H, d3);

    //TM1637_display_colon(true);
    //_delay_ms(200);
    //TM1637_display_colon(false);
}

*/

uint8_t value = 95;

int main() {
  TM1637_init();
  RCTSwitch_setup(PIN_TRANSMITTER);
  sbi(DDRB, PIN_LED_SEND);
  while ( 1 ) {
    sbi(PORTB, PIN_LED_SEND);
    RCTSwitch_sendbyte(PIN_TRANSMITTER, value, 10);
    _delay_ms(100);
    cbi(PORTB, PIN_LED_SEND);
    _delay_ms(900);
    value += 1;
  } 
  return 0;
}

