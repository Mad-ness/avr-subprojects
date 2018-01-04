
#include <Arduino.h>
#include <TM1637Display.h> // https://github.com/avishorp/TM1637

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


#define pin_PHOTOR_TUNER  A0
#define pin_DISPLAY_CLK   7
#define pin_DISPLAY_DIO   8

TM1637Display display(pin_DISPLAY_CLK, pin_DISPLAY_DIO);

void setup() {
  // put your setup code here, to run once:
  display.setBrightness(0x0f);
}

void loop() {
  // put your main code here, to run repeatedly:

  for (int i=0; i<9999; i++) {
    display.showNumberDec(i, false, 3, 1);
    delay(100);
  }
}
