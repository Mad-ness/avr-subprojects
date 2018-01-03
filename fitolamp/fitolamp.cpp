#include "C:\Users\papa\Documents\Arduino\libraries\OneWire\OneWire.h"
#include <iarduino_RTC.h>
#include <SevenSegmentExtended.h>
#include <DallasTemperature.h>


#define PHOTOR_TUNER_PIN A0
#define PHOTO_RESTOR_PIN A1
#define disp_CLK 4
#define disp_DIO 5
#define DISPL_DELAY 4
#define RELAY_PIN 2
#define TEMP_SENSOR_PIN 6
#define TEMP_SENSOR_POWER_PIN 7

OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature temp_sensors(&oneWire);
iarduino_RTC time(RTC_DS1307);
SevenSegmentExtended disp(disp_CLK, disp_DIO);

void setup() {
    delay(300);
    Serial.begin(9600);
    time.begin();
    pinMode(2, OUTPUT);
    pinMode(PHOTO_RESTOR_PIN, INPUT);
    pinMode(PHOTOR_TUNER_PIN, INPUT);
    pinMode(TEMP_SENSOR_POWER_PIN, OUTPUT);
    digitalWrite(TEMP_SENSOR_POWER_PIN, HIGH);
    disp.begin();
    disp.setBacklight(100);
    disp.print("INIT");
    temp_sensors.begin();
}

uint16_t prev_photores_value = 0;
int display_value = 0x0;
uint16_t photores_threshold = 400;
bool enable_photobus = true;
//int run_time = 0; //seconds

void displayNumber(const uint16_t num)
{
  //if (num % 10 < 1) disp.print("  ");
  //if (num % 100 < 10) disp.print(" ");
  disp.clear();
  disp.setColonOn(false);
  disp.print(num);
}

void displayTime(const uint8_t hours, const uint8_t minutes)
{
  //disp.setColonOn(true);
  disp.printTime(hours, minutes);
}

uint16_t getAnalogValue(const uint8_t pin) {
    #define ITERS 5
    uint16_t value = 0;
    for (int i=0; i<ITERS; i++) {
      value += analogRead(pin);
      delay(10); }
    return value / 5;
}

uint16_t setupPhotoResistorThreshold() {
    uint16_t old_val = photores_threshold;
    uint8_t counter = 0;
    uint16_t old_millis = millis();
    uint16_t new_millis = old_millis;
    while (counter < 50)
    {
      uint16_t new_val = getAnalogValue(PHOTOR_TUNER_PIN);
      if (abs(new_val - old_val) > 10)
      {
        photores_threshold = new_val;
        old_val = new_val;
        counter = 0;
        displayNumber(photores_threshold);
        delay(50);
    Serial.print("New threshold is configured as ");
    Serial.print(photores_threshold);
    Serial.print(" - ");
    Serial.print(photores_threshold);
    Serial.println("%");
      } else
        counter++;

      new_millis = millis();
      if (new_millis - old_millis > 4000)
        break;
      else
        counter = 0;
      //old_millis = new_millis;
    }
    return old_val;
}


void displayTemperature() {
  temp_sensors.requestTemperatures();
  float temp = temp_sensors.getTempCByIndex(0);
  int count = 0;
  Serial.print("Current temperature is ");
  Serial.print(temp); Serial.println(" C");
  while (count < 8)  // wait for 4 seconds
  {
    displayNumber(uint16_t(temp*100));
    delay(100);
    temp_sensors.requestTemperatures();
    temp = temp_sensors.getTempCByIndex(0);
    count++;
  }
}

void displayCurrTime() {
  Serial.print("System time ");
  Serial.println(time.gettime("d-m-Y, H:i:s, D")); // выводим время
  delay(10);
  int count = 0;
  while (count < 10) {
    time.gettime();
    displayTime(time.Hours, time.minutes);
    delay(100);
    count++;
  }
}

void loop(){
    Serial.print(" ---- ");
    displayCurrTime();
    displayTemperature();
    photores_threshold = setupPhotoResistorThreshold();
    displayPhotoResistorValue();
}

void displayPhotoResistorValue() {
    int counter = 0;
    while (counter++ <= 20) {

    uint16_t photores_value = 1023 - getAnalogValue(PHOTO_RESTOR_PIN);
    //uint16_t ph_perc = 100 - (uint16_t)((uint16_t)(photores_value*100)/1023);
    Serial.print("Current resistance of a photo sensor is ");
    Serial.print(photores_value);
//    Serial.print(", ");
//    Serial.print(ph_perc);
    Serial.print(". Threshold is ");
    Serial.print(photores_threshold);
    Serial.println(".");
      if  ((time.Hours >= 4 && time.Hours < 22) || (time.Hours == 22 && time.minutes <= 40))
      {
        displayNumber(photores_value);
        if (prev_photores_value != 0 && photores_value != prev_photores_value)
        {
            if (abs(photores_value - photores_threshold) > 50)
            {
              enable_photobus = true ? photores_value < photores_threshold : false;
              counter--;
            }
            prev_photores_value = photores_value;
        } else {
            enable_photobus = photores_value < photores_threshold ? true : false;
        }
//        Serial.print("This is daylight ");
        if (enable_photobus)
        {
//            Serial.println("but the natural light is not good. Use a lamp");
            digitalWrite(RELAY_PIN, LOW);
        } else {
//            Serial.println("but the natural light is fine");
            digitalWrite(RELAY_PIN, HIGH);
        }
      } else {
//        Serial.println("This is time to sleep");
        digitalWrite(RELAY_PIN, HIGH);
      }
      delay(250);
   }
}
