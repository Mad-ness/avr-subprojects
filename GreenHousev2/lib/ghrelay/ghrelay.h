#ifndef __GHRELAY_H__
#define __GHRELAY_H__

#include <Arduino.h>
#include <ghobject.h>

#define RELAY_ADDR_USE_EEPROM           0x0     // if set to 1, it means use EEPROM
#define RELAY_ADDR_OPERTIME             (RELAY_ADDR_USE_EEPROM + 1)
#define RELAY_ADDR_MODE                 (RELAY_ADDR_OPERTIME + 4) // 4 bytes = size(m_startTime) + sizeof(m_lenght)
#define RELAY_ADDR_STATE                (RELAY_ADDR_MODE + sizeof(bool))

#define AIR_RELAY_STATE_UNDEF           -1
#define AIR_RELAY_STATE_OFF             0x00
#define AIR_RELAY_STATE_RUN             0x01
#define AIR_RELAY_MODE_AUTO             0x00
#define AIR_RELAY_MODE_MANUAL           0x01
#define AIR_RELAY_ID_0                  0x00
#define AIR_RELAY_ID_1                  0x01
#define AIR_RELAY_ID_2                  0x02
#define AIR_RELAY_ID_3                  0x03


class GHRTC;

class GHRelayBase: public GHObjectEx {
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
    uint16_t m_lastCheck;
    uint16_t m_lenght; // in minutes
    GHRTC *m_rtc;
    bool m_workStarted;
    bool m_workStateFlag;
    int8_t m_mode;      // m_mode == 0 - by schedule, 1 - manual
public:
    GHRelayBase(const int8_t pin);
    void loop();
    void onBegin();
    void onEnd();
    bool isOn();
    void workStateIsHIGH(const bool);
    GHRelayBase::Time *getStartTime();
    const uint16_t *getLength();
    void setRTC(GHRTC *rtc);
    void setOperationalTime(const uint8_t hour, const uint8_t minute, const uint16_t minutes);
    void getOperationalTime(uint8_t *hour, uint8_t *minute, uint16_t *length);
};

class GHRelay: public GHRelayBase {
public:
    GHRelay(const int8_t pin);
};



#endif // __GHRELAY_H__
