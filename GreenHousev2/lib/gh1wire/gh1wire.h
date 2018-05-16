#ifndef __GH1WIRE_H__
#define __GH1WIRE_H__

#include <Arduino.h>
#include <OneWire.h>
//#include <DallasTemperature.h>
#include <ghobject.h>

class GH1Wire: public GHObject {
private:
    OneWire m_1wire;
    // Pass our oneWire reference to Dallas Temperature.
//    DallasTemperature m_sensors;

    // arrays to hold device addresses
   // DeviceAddress insideThermometer, outsideThermometer;
public:
    GH1Wire(const int8_t pin);
    void setup();
    void loop();
    float getTempCByIndex(uint8_t);
    void requestTemperatures(const uint8_t index);
};

#endif // __GH1WIRE_H__
