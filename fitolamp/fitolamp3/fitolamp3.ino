
#include <Arduino.h>
#include <OneWire.h>
#include <Wire.h>
#include <TM1637Display.h> // https://github.com/avishorp/TM1637
#include <DallasTemperature.h>  // https://create.arduino.cc/projecthub/TheGadgetBoy/ds18b20-digital-temperature-sensor-and-arduino-9cc806
#include <ResponsiveAnalogRead.h> // https://github.com/dxinteractive/ResponsiveAnalogRead
#include <RTClib.h>


/*       A
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
 *       SEG_G | SEG_E
 *       The letter "o"
 *       SEG_G | SEG_C | SEG_E | SEG_D
 *
 *       The letter "O"
 *       SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F
 *       The letter "f"
 *       SEG_A | SEG_F | SEG_E | SEG_G
 *       The letter "n"
 *       SEG_E | SEG_G | SEG_C
 *       The letter "R"
 *       SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G | SEG_F
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

//#define logmsg(x)         Serial.print(x);
//#define logmsgln(x)       Serial.println(x);
#define logmsg(x)         ;
#define logmsgln(x)       ;
#define RELAY_ON          LOW
#define RELAY_OFF         HIGH

enum class           DisplayInfo { Time, PhTuner, PhSensor, Temperature, Relay };
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
  bool               phototuning_mode = false;
  int                relayState = RELAY_OFF;
} LightInfo;


void setup() {
  logmsgln("Starting the device ...");
  delay(300);
  Serial.begin(9600);
  display.setBrightness(0x0f);
  tuner_length = display_length = millis();
  onewire_sensors.begin();
  photor_tuner.update();
  phsensor.update();
  pinMode(pin_RELAY, OUTPUT);
  pinMode(pin_TEMP_POWER, OUTPUT);  // I use it as a power source +5V
  digitalWrite(pin_TEMP_POWER, HIGH);
  digitalWrite(pin_RELAY, LightInfo.relayState);
  LightInfo.light_level = phsensor.getValue()/10.23;
  LightInfo.tuner_level = photor_tuner.getValue()/10.23;
  //Wire.begin();
  RTC.begin();
  system_time = RTC.now();
}


DisplayInfo getNextDisplayItem() {
  //return DisplayInfo::PhSensor;
  DisplayInfo result = display_value;
    switch (display_value) {
      case DisplayInfo::Time:
        result = DisplayInfo::Relay;
        logmsgln(" >> Switched to Temperature");
        break;
      case DisplayInfo::Relay:
        result = DisplayInfo::Temperature;
        logmsgln(" >> Switched to Relay");
        break;
      case DisplayInfo::Temperature:
        result = DisplayInfo::PhSensor;
        logmsgln(" >> Switched to Photo Sensor");
        break;
      case DisplayInfo::PhSensor:
        result = DisplayInfo::PhTuner;
        logmsgln(" >> Switched to Light Tuner");
        break;
      case DisplayInfo::PhTuner:
        result = DisplayInfo::Time;
        logmsgln(" >> Switched to System Time");
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
  if (running_time % 100 == 0) {
      phsensor.update();
      photor_tuner.update();
      system_time = RTC.now();


      new_light_level = 100 - phsensor.getValue()/10.23;
      new_tuner_level = 100 - photor_tuner.getValue()/10.23;

      if (new_light_level != LightInfo.light_level || new_tuner_level != LightInfo.tuner_level) {
        logmsg("New tuner ");
        logmsg(new_tuner_level);
        logmsg(" or sensor ");
        logmsg(new_light_level);
        logmsgln(" values");
      }


      if (photor_tuner.hasChanged() && abs(new_tuner_level - LightInfo.tuner_level) > 0) {
        // if 1st time rotating tuner
        if (!LightInfo.phototuning_mode) {
          LightInfo.phototuning_mode = true;
          display_old_value = display_value;
          display_value = DisplayInfo::PhTuner;
        }
        tuner_length = running_time;
        // if tuner is not rotating more and waited for a while
      } else if (LightInfo.phototuning_mode && (running_time - tuner_length) > 3000) {
        LightInfo.phototuning_mode = false;
        display_value = display_old_value;
        // if no tuner is rotating, and need to change what to display
      } else if (LightInfo.phototuning_mode != true && (running_time - display_length > display_change_delay)) {
        display_value = getNextDisplayItem();
        display_length = running_time;
      }
        // If no tuner is rotating
      const byte latest_hour = 22, latest_minute = 30;
      if ((system_time.hour() > 4 && system_time.hour() < latest_hour) ||
          (system_time.hour() == latest_hour && system_time.minute() < latest_minute)) {
         if (LightInfo.phototuning_mode) {
           LightInfo.relayState = RELAY_ON ? new_tuner_level >= new_light_level : RELAY_OFF;
           logmsgln("I'm here");
         } else {
           //if (LightInfo.has_crossed_threshold == false) {
             if ((new_light_level > new_tuner_level) && (LightInfo.light_level <= LightInfo.tuner_level) ||
                 (new_light_level <= new_tuner_level) && (LightInfo.light_level > LightInfo.tuner_level)) {
               logmsgln("Threshold is crossed");
               LightInfo.length_changed_light = running_time;
               LightInfo.has_crossed_threshold = true;
             }
             
             if ((LightInfo.has_crossed_threshold && running_time - LightInfo.length_changed_light > 4000) ||
                 (LightInfo.has_crossed_threshold == false)) {
                 LightInfo.has_crossed_threshold = false;
                 int relay_state; //= RELAY_ON ? new_tuner_level >= new_light_level : RELAY_OFF;
                 if (new_light_level <= new_tuner_level) 
                   relay_state = RELAY_ON;
                 else 
                   relay_state = RELAY_OFF;
                 LightInfo.relayState = relay_state;
               //logmsgln("Time expired, it's time to set relay");
             }

         }
      } else {
        LightInfo.relayState = RELAY_OFF;
        //logmsgln("Time to sleep");
      }
      
      LightInfo.tuner_level = new_tuner_level;
      LightInfo.light_level = new_light_level;

      if (running_time % 500 == 0)
        digitalWrite(pin_RELAY, LightInfo.relayState);




    // if a tuner is rotating
    // set that a display should show the changing values
    /*
    if (photor_tuner.hasChanged() && abs(new_tuner_level - LightInfo.tuner_level) > 0) {
      if (LightInfo.phototuning_mode != true) {
        LightInfo.phototuning_mode = true;
        display_old_value = display_value;
        display_value = DisplayInfo::PhTuner;
        logmsgln("Tuner settings changed");
        logmsgln("Changed PhotoTuning mode to True");

      }
      tuner_length = running_time;
      logmsg("New tuner value is ");
      logmsgln(new_tuner_level);
    } else if (LightInfo.phototuning_mode && (running_time - tuner_length) > 3000) {
      display_value = display_old_value;
      LightInfo.phototuning_mode = false;
      logmsgln("Changed PhotoTuning mode to False because of time expired");
    }
    */



/*
    // Detecting whether we should switch a relay state
    if ((new_light_level > new_tuner_level) && (LightInfo.light_level <= LightInfo.tuner_level) ||
        (new_light_level <= new_tuner_level) && (LightInfo.light_level > LightInfo.tuner_level)) {
      LightInfo.length_changed_light = running_time;
      LightInfo.has_crossed_threshold = true;
      logmsgln("Threshold is crossed out");
    }
*/
  /*
    LightInfo.tuner_level = new_tuner_level;
    LightInfo.light_level = new_light_level;

    if ((system_time.hour() > 4 && system_time.hour() < 23) ||
        (system_time.hour() == 23 && system_time.minute() < 59)) {
      if (LightInfo.has_crossed_threshold) {
        if  ((running_time - LightInfo.length_changed_light) > 3000) {
          LightInfo.relayState = RELAY_ON ? new_tuner_level >= new_light_level : RELAY_OFF;
          LightInfo.has_crossed_threshold = false;
          logmsg("Changing relay state to ");
          logmsgln(LightInfo.relayState);
        } else {
          logmsgln("I'm here");
          LightInfo.relayState = RELAY_ON ? new_tuner_level >= new_light_level : RELAY_OFF;
        }
      } else {
        LightInfo.relayState = RELAY_ON ? new_tuner_level >= new_light_level : RELAY_OFF;  
        logmsg("Testing this expression as tuner "); 
        logmsg(new_tuner_level);     
        logmsg(", sensor ");
        logmsgln(new_light_level); 
      }
      //logmsg("Write relay state as ");
      //logmsgln(LightInfo.relayState);
    } else {
      LightInfo.relayState = RELAY_OFF;
      logmsgln("Time to sleep");
    }

    // do it once per second just to make sure that 
    // actual state is applied in a relay
    if (running_time % 3000 == 0) {
      logmsg("Set relay in a required state ");
      logmsgln(LightInfo.relayState);
      digitalWrite(pin_RELAY, LightInfo.relayState);
    }

    if (LightInfo.phototuning_mode != true && (running_time - display_length > display_change_delay)) {
      logmsgln("===[ Next iteration passed ]===");
      display_value = getNextDisplayItem();
      display_length = running_time;
    }
    */
  }

  switch (display_value) {
    case DisplayInfo::Time:
      if (running_time % 500 == 0) {
        if (RTC.isrunning()) {
          display.showNumberDecEx(system_time.hour()*100 + system_time.minute(), 0, false);
        } else {
          char data[4];
          data[0] = SEG_F | SEG_G | SEG_E | SEG_D;
          data[1] = SEG_A | SEG_F | SEG_G | SEG_E | SEG_D;
          data[2] = SEG_G | SEG_E;
          data[3] = SEG_G | SEG_E;
          display.setSegments(data, 4, 0);
        }
      } // refresh display every half second (500 milliseconds)
      break;

    case DisplayInfo::PhTuner:
      if (running_time % 100 == 0) {
        displayLabeledNumber(LightInfo.tuner_level, 2, SEG_A | SEG_F | SEG_G | SEG_C | SEG_D);
      }
      break;

    case DisplayInfo::PhSensor:
      if (running_time % 200 == 0) {
        displayLabeledNumber(new_light_level, 2, SEG_F | SEG_E | SEG_D);
      }
      break;

    case DisplayInfo::Relay:
      if (running_time % 200 == 0) {
        char data[4] = {SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G | SEG_F, 0x0, 0x0, 0x0};
        if (LightInfo.relayState == RELAY_ON) {
          data[2] = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
          data[3] = SEG_E | SEG_G | SEG_C;
        } else if (LightInfo.relayState == RELAY_OFF) {
          data[1] = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
          data[2] = SEG_A | SEG_F | SEG_E | SEG_G;
          data[3] = SEG_A | SEG_F | SEG_E | SEG_G;
        } else {
          data[1] = SEG_A | SEG_F | SEG_G | SEG_E | SEG_D;
          data[2] = SEG_G | SEG_E;
          data[3] = SEG_G | SEG_E;
        }
        //logmsg("Relay state is ");
        //logmsgln(LightInfo.relayState);
        display.setSegments(data, 4, 0);
      }
      break;

    case DisplayInfo::Temperature:
      if (running_time % 500 == 0) {
        onewire_sensors.requestTemperatures();
        float tempsensor_value = onewire_sensors.getTempCByIndex(0);
        //logmsg("Read temperature ");
        //logmsgln(tempsensor_value);
        displayLabeledNumber(tempsensor_value*10, 3, SEG_A | SEG_B | SEG_F | SEG_G);
      }
      break;

    default:
      display_value = DisplayInfo::Time;
      break;
  }
}
