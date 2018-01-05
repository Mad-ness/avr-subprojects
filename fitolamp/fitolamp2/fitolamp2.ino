
#include <Arduino.h>
#include <TM1637Display.h> // https://github.com/avishorp/TM1637

#include <OneWire.h> 
#include <DallasTemperature.h>  // https://create.arduino.cc/projecthub/TheGadgetBoy/ds18b20-digital-temperature-sensor-and-arduino-9cc806

/*      A
 *    +----+
 *  F |  G | B
 *    +----+
 *  E |    | C
 *    +----+
 *       D
 *       
 *       Celsius symbol
 *       SEG_A | SEG_B | SEG_F | SEG_G
 */    

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif



#define pin_PHOTO_SENSOR  A1
#define pin_PHOTOR_TUNER  A0
#define pin_DISPLAY_CLK   7
#define pin_DISPLAY_DIO   8
#define pin_ONEWIRE       9
#define pin_TEMPSENSOR    pin_ONEWIRE
/*
#define DINFO_TIME        0x1
#define DINFO_TUNER       0x2   // photosensor threshold value
#define DINFO_PSENSOR     0x3   // photosensor value
#define DINFO_TEMP        0x4
*/
enum class        DisplayInfo { Time, PhTuner, PhSensor, Temperature };
DisplayInfo       display_value = DisplayInfo::Time; // Initial value should be set
OneWire           oneWire(pin_ONEWIRE);      // for DS18B20
DallasTemperature onewire_sensors(&oneWire);  // for DS18B20


word display_change_delay = 4000; // milliseconds
word display_length = 0;
word phtuner_value = 400;
uint16_t phsensor_value = 400;
float tempsensor_value = 20;


TM1637Display display(pin_DISPLAY_CLK, pin_DISPLAY_DIO);

void setup() {
  Serial.begin(9600);
  display.setBrightness(0x0f);
  display_length = millis();
  onewire_sensors.begin();
  pinMode(pin_PHOTOR_TUNER, INPUT);
  pinMode(pin_PHOTO_SENSOR, INPUT);
}


DisplayInfo getNextDisplayItem() {
  DisplayInfo result = display_value;
    switch (display_value) {
      case DisplayInfo::Time:
        result = DisplayInfo::Temperature;
        Serial.println(" >> Switched to Temperature");
        break;
      case DisplayInfo::Temperature:
        result = DisplayInfo::PhSensor;
        Serial.println(" >> Switched to Photo Sensor");
        break;
      case DisplayInfo::PhSensor:
        result = DisplayInfo::Time;
        Serial.println(" >> Switched to System Time");
        break;
    }
    return result;
}

void loop() {
  word running_time = millis();

  switch (display_value) {
    case DisplayInfo::Time:
      if (running_time % 1000 == 0) {
        //Serial.println("  >>> displaying system time");
      } // refresh display every 1000 milliseconds (1 second)
       // display.showNumberDec(/* ... */);
      break;

    case DisplayInfo::PhTuner:
      if (running_time % 100 == 0) {
        //Serial.println("  >>> displaying photo tuner value");
        //phtuner_value = analogRead(pin_PHOTOR_TUNER);
        // phtuner_value 
        phtuner_value += 50;
        if (phtuner_value > 1023)
          phtuner_value = 0;
        Serial.print(phtuner_value);
        Serial.print(" => ");
        Serial.print(phtuner_value/10.23);
        Serial.println("%");
      }
      break;

    case DisplayInfo::PhSensor:
      if (running_time % 500 == 0) {
        // phsensor_value = analogRead(pin_PHOTO_SENSOR);
        phsensor_value += 50;
        if (phsensor_value > 1023)
          phsensor_value = 0;
        Serial.print(phsensor_value);
        Serial.print(" => ");
        Serial.print(phsensor_value/10.23);
        Serial.println("%");
      }
      break;

    case DisplayInfo::Temperature:
      if (running_time % 500 == 0) {
        onewire_sensors.requestTemperatures();
        tempsensor_value = onewire_sensors.getTempCByIndex(0);
        display.showNumberDec(word(tempsensor_value*10), false);
        display.setSegments(SEG_A | SEG_B | SEG_F | SEG_G, 3, 1);
        Serial.println(tempsensor_value);
        //Serial.println("  >>> displaying temperature ");
      }
      break;

    default:
      display_value = DisplayInfo::Time;
      break;
  }

 if (running_time - display_length > display_change_delay) {
    Serial.println("===[ Next iteration passed ]===");
    display_value = getNextDisplayItem();
    display_length = running_time;
  }
}

