#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <Arduino.h>
#include <ghobject.h>
#include <ghdisplay.h>
#include <ghrtc.h>
#include <ghrelay.h>
#include <gh1wire.h>
#include <ghbuttonset.h>

const int8_t pin_DISP_SID  = 5; /* Serial Data In (Din) */
const int8_t pin_DISP_SCLK = 6; /* SCLK, clock (SCK, CLK) */
const int8_t pin_DISP_DC   = 4; /* D/C, Data/Command Selector */
const int8_t pin_DISP_RST  = 2;
const int8_t pin_DISP_CS   = 3; /* CS, Chip Select */
const int8_t pin_DISP_BL   = 7;

const int8_t pin_1Wire     = 8;
const int8_t pin_Relay_1   = 12;
const int8_t pin_ButtonSet = A0;

// const int8_t pin_RELAY_1   = 9;
// #define pin_RELAY_2   10
// #define pin_RELAY_3   11
// #define pin_RELAY_4   12

class GHDisplay;

class Manager: public GHObject {
private:
    GHDisplay m_display;
    GHRTC m_rtc;
    GH1Wire m_1wire;
    GHRelay m_relay1;
    GHButtonSet m_buttonSet;
    /*
    Relay m_relay1(pin_RELAY_1);
    Relay m_relay2(pin_RELAY_2);
    Relay m_relay3(pin_RELAY_3);
    Relay m_relay4(pin_RELAY_4);
    */
public:
    Manager();
    GHDisplay display() { return this->m_display; }
    void setup();
    void loop();

};

#endif // __MANAGER_H__
