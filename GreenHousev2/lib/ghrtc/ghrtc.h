#ifndef __GHRTC_H__
#define __GHRTC_H__

#include <Arduino.h>
#include <ghobject.h>
#include <Wire.h>
#include <DS1307.h>

struct GHRTCDateTime {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

class GHRTC: public GHObject {
private:
    DS1307 m_rtc;
public:
    GHRTC();
    DS1307 *rtc();
    void setup();
    void loop();
    bool isAvailable();
    void getDateTime(GHRTCDateTime *dt);
};

#endif // __GHRTC_H__
