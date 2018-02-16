#include <avr/io.h>
#include <util/delay.h>
#include <tm1637.h> // doesn't fit into the attiny13 flash memory
#include <common.h>
//#include <rctswitch.h>

#define PIN_LED_SEND PB0
#define PIN_TRANSMITTER PB4
#define pulse_len 350

static uint8_t data;
/*
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
*/

void display_test(const uint8_t number) {
    uint8_t d0 = number / 1000;
    uint8_t d1 = (number - d0*1000) / 100;
    uint8_t d2 = (number - d0*1000 - d1*100)/10;
    uint8_t d3 = (number - d0*1000 - d1*100 - d2*10);
    TM1637_display_digit(TM1637_SET_ADR_00H, d0);
    TM1637_display_digit(TM1637_SET_ADR_01H, d1);
    TM1637_display_digit(TM1637_SET_ADR_02H, d2);
    TM1637_display_digit(TM1637_SET_ADR_03H, d3);
}

int main() {
  TM1637_init();
  sbi(DDRB, PIN_LED_SEND);
  //RCTSwitch_setup(PIN_TRANSMITTER);

  while ( 1 ) {
    display_test(adc_read());
  }


/*
  while ( 1 ) {
    sbi(PORTB, PIN_LED_SEND);
    uint8_t data = (uint8_t)(adc_read()/10.23);
    //RCTSwitch_sendbyte(PIN_TRANSMITTER, data, 10);
    sendbyte(PIN_TRANSMITTER);
    _delay_ms(100);
    cbi(PORTB, PIN_LED_SEND);
    _delay_ms(900);
  } 
*/
  return 0;
}

