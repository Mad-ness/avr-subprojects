
#include <Arduino.h>
#include <OneWire.h> 
#include <Wire.h>
#include <TM1637Display.h> // https://github.com/avishorp/TM1637
#include <DallasTemperature.h>  // https://create.arduino.cc/projecthub/TheGadgetBoy/ds18b20-digital-temperature-sensor-and-arduino-9cc806
#include <ResponsiveAnalogRead.h> // https://github.com/dxinteractive/ResponsiveAnalogRead
#include <RTClib.h>

 
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
 *       The letter "S"
 *       SEG_A | SEG_F | SEG_G | SEG_C | SEG_D
 *       The letter "t"
 *       SEG_F | SEG_G | SEG_E | SEG_D
 *       The letter "L"
 *       SEG_F | SEG_E | SEG_D
 *       The letter "E"
 *       SEG_A | SEG_F | SEG_G | SEG_E | SEG_D
 *       The letter "r"
 *       SEG_G | SEG_E | SEG_D
 *       The letter "o"
 *       SEG_G | SEG_C | SEG_E | SEG_D
 */    


#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


#define pin_PHOTO_SENSOR  A1  // photoresistor is here
#define pin_PHOTOR_TUNER  A0  // a potentiometr here
#define pin_DISPLAY_CLK   4
#define pin_DISPLAY_DIO   5
#define pin_ONEWIRE       6
#define pin_TEMPSENSOR    pin_ONEWIRE
#define pin_RELAY         2
#define pin_TEMP_POWER    7

enum class           DisplayInfo { Time, PhTuner, PhSensor, Temperature };
DisplayInfo          display_value = DisplayInfo::Time; // Initial value should be set
DisplayInfo          display_old_value = DisplayInfo::PhTuner;
OneWire              onewire(pin_ONEWIRE);       // for DS18B20
TM1637Display        display(pin_DISPLAY_CLK, pin_DISPLAY_DIO);
DallasTemperature    onewire_sensors(&onewire);  // for DS18B20
ResponsiveAnalogRead photor_tuner(pin_PHOTOR_TUNER, true);
ResponsiveAnalogRead phsensor(pin_PHOTO_SENSOR, true);
RTC_DS1307           RTC;
DateTime             system_time;


word                 display_change_delay = 4000; // milliseconds
word                 display_length = 0;
uint16_t             tuner_length = 0;

struct LightInfo_st {
  uint8_t            light_level = 0;
  uint8_t            tuner_level = 0;  
  bool               has_crossed_threshold = false;
  uint16_t           length_changed_light = 0;
} LightInfo;


void setup() {
  delay(300);
  Serial.begin(9600);
  display.setBrightness(0x0f);
  tuner_length = display_length = millis();
  onewire_sensors.begin();
  photor_tuner.update();
  phsensor.update();
  pinMode(pin_RELAY, OUTPUT);  
  pinMode(pin_TEMP_POWER, OUTPUT);  // I use it as a power source +5V
  LightInfo.light_level = phsensor.getValue()/10.23;
  LightInfo.tuner_level = photor_tuner.getValue()/10.23;
  //Wire.begin();
  RTC.begin();
  system_time = RTC.now();
}


DisplayInfo getNextDisplayItem() {
//  return DisplayInfo::PhSensor;
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


/**
 * ch - a symbol that is being displayed as zero position 
 * number - how many digits to display, starting from the end
 * len - how many digits are being displayed
 */
void displayLabeledNumber(word number, const int len, char ch = 0x0) {
  byte data[4];
  byte d0 = number / 1000;
  byte d1 = (number - d0*1000) / 100;
  byte d2 = (number - d0*1000 - d1*100) / 10;
  byte d3 = (number - d0*1000 - d1*100 - d2*10);
  data[0] = (len == 4) ? (display.encodeDigit(d0)) : ch;
  data[1] = (len >= 3) ? (display.encodeDigit(d1)) : 0x0;
  data[2] = (len >= 2) ? (display.encodeDigit(d2)) : 0x0;
  data[3] = (len >= 1) ? (display.encodeDigit(d3)) : 0x0;
  display.setSegments(data, 4, 0);
}


uint8_t new_light_level;
uint8_t new_tuner_level;
bool light_level_changed = false;

void loop() {
  uint16_t running_time = millis();

  // Read PhotoSensor and
  // PhotoTuner values 10 times per sec
  if (running_time % 150 == 0) {
      // read the sensor and tuner values
      phsensor.update();
      photor_tuner.update();
      system_time = RTC.now();
      int num = onewire_sensors.getDeviceCount();
      Serial.print("Found DS18xx devices on bus - "); Serial.println(num);
      //Serial.print("Hours "); Serial.print(system_time.hour());
      //Serial.print(", minutes "); Serial.println(system_time.minute());

      new_light_level = 100 - phsensor.getValue()/10.23;
      new_tuner_level = 100 - photor_tuner.getValue()/10.23;

    // Enable displaying the Tuner values on a display
    // only when the tuner is rotating
    if (photor_tuner.hasChanged() && abs(new_tuner_level - LightInfo.tuner_level) > 1 ) {
      if (display_value != DisplayInfo::PhTuner) {
        display_old_value = display_value;
        display_value = DisplayInfo::PhTuner;
//        Serial.println("Switched to display Tuner values");
      }
      tuner_length = running_time;
//      Serial.print(tuner_length); Serial.print(" - "); Serial.println(running_time);
    }
    if (display_value == DisplayInfo::PhTuner && running_time - tuner_length > 3000) {
      display_value = display_old_value;
      Serial.println(" - Tuner is not rotating anymore");
    }
  }

  switch (display_value) {
    case DisplayInfo::Time:
      if (running_time % 500 == 0) {
        if (RTC.isrunning()) {
          display.showNumberDecEx(system_time.hour()*100 + system_time.minute(), 0, false);
        } else {
          char data[4];
          data[0] = SEG_A | SEG_F | SEG_G | SEG_E | SEG_D;
          data[1] = SEG_G | SEG_E | SEG_D;
          data[2] = SEG_G | SEG_C | SEG_E | SEG_D;
          data[3] = SEG_G | SEG_E | SEG_D;
          display.setSegments(data, 4, 0);
        }
      } // refresh display every half second (500 milliseconds)
      break;

    case DisplayInfo::PhTuner:
      if (running_time % 200 == 0) {
        displayLabeledNumber(new_tuner_level, 2, SEG_A | SEG_F | SEG_G | SEG_C | SEG_D);
        //Serial.println("DISPLAY TUNER VALUE");
      }
      break;

    case DisplayInfo::PhSensor:
      if (running_time % 200 == 0) {
        displayLabeledNumber(new_light_level, 2, SEG_F | SEG_E | SEG_D);
      }
      break;

    case DisplayInfo::Temperature:
      if (running_time % 500 == 0) {
        onewire_sensors.requestTemperatures();
        float tempsensor_value = onewire_sensors.getTempCByIndex(0);
        Serial.print("Read temperature ");
        Serial.println(tempsensor_value);
        displayLabeledNumber(tempsensor_value*10, 3, SEG_A | SEG_B | SEG_F | SEG_G);
      }
      break;

    default:
      display_value = DisplayInfo::Time;
      break;
  }


  // Detecting whether a light level or a tuner value has changed)
  if ((abs(new_light_level - LightInfo.light_level) > 1 ||
      abs(new_tuner_level - LightInfo.tuner_level) > 1) &&
      running_time % 200 == 0) {
    Serial.print(new_light_level); Serial.print(" "); Serial.println(LightInfo.light_level);
    Serial.print(new_tuner_level); Serial.print(" "); Serial.println(LightInfo.tuner_level);
    LightInfo.light_level = new_light_level;
    LightInfo.tuner_level = new_tuner_level;
    LightInfo.length_changed_light = running_time;
    LightInfo.has_crossed_threshold = true;
    Serial.println("Detected Tuner or PhotoSensor changes");
  }
  if ((system_time.hour() > 4 && system_time.hour() < 22) || 
      (system_time.hour() == 22 && system_time.minute() < 40)) {
    if (LightInfo.has_crossed_threshold && ((running_time - LightInfo.length_changed_light) > 3000)) {
      digitalWrite(pin_RELAY, HIGH ? photor_tuner.getValue() >= phsensor.getValue() : LOW);
      LightInfo.has_crossed_threshold = false;
    }
  } else {
    digitalWrite(pin_RELAY, HIGH);
    //Serial.println("Make it off");
  }



 if (running_time - display_length > display_change_delay) {
//    Serial.println("===[ Next iteration passed ]===");
    display_value = getNextDisplayItem();
    display_length = running_time;
  }
}


