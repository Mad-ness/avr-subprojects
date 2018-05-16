#ifndef __GHRELAY_H__
#define __GHRELAY_H__

#include <Arduino.h>
#include <ghobject.h>

class GHRTC;

class GHRelayBase: public GHObject {
    int8_t m_pin;
    //int8_t m_pinState;
    struct Time {
        uint8_t hour;
        uint8_t minute;
    } m_startTime;
    struct {
        uint8_t hour;
        uint8_t minute;
    } m_stopTime;
    unsigned long m_lastCheck;
    uint16_t m_lenght; // in minutes
    GHRTC *m_rtc;
    bool m_workStarted;
    bool m_workStateFlag;
public:
    GHRelayBase(const int8_t pin);
    void loop();
    void onBegin();
    void onEnd();
    bool state();
    void workStateIsHIGH(const bool);
    GHRelayBase::Time *getStartTime();
    const int *getLength();
    void setRTC(GHRTC *rtc);
    void setOperationalTime(const uint8_t hour, const uint8_t minute, const uint16_t minutes);
    void getOperationalTime(uint8_t *hour, uint8_t *minute, uint16_t *length);
    /** This is a shift relatevely the base address,
    * where the arduino will write the start time + lenght of operational time.
    * For each of instances it should be
    * set to a different value.
    * Example: relay1.setEEPROMAddress(0x0)
    * relay2.setEEPROMAddress(0x1)
    */
    void setEEPROMAddress(const int addr);
};

class GHRelay: public GHRelayBase {
public:
    GHRelay(const int8_t pin);
};

#endif // __GHRELAY_H__
