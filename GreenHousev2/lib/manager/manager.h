#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <Arduino.h>
#include <ghobject.h>
#include <ghdisplay.h>
#include <ghrtc.h>
#include <ghrelay.h>
#include <gh1wire.h>
#ifdef ENABLE_BUTTONSET
#include <ghbuttonset.h>
#endif // ENABLE_BUTTONSET
#include <ghair.h>

const int8_t pin_DISP_SID  = 5; /* Serial Data In (Din) */
const int8_t pin_DISP_SCLK = 6; /* SCLK, clock (SCK, CLK) */
const int8_t pin_DISP_DC   = 4; /* D/C, Data/Command Selector */
const int8_t pin_DISP_RST  = 2;
const int8_t pin_DISP_CS   = 3; /* CS, Chip Select */
const int8_t pin_DISP_BL   = 7;

const int8_t pin_1Wire     = 8;
const int8_t pin_Relay_1   = 12;
const int8_t pin_ButtonSet = A0;

#define pin_Air_CE          1
#define pin_Air_CSN         2
const byte AirPeers[][AIR_ADDRESS_SIZE] = { "1Node", "2Node" };


//
// See special Air constants in the ghairdefs.h file
//

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
#ifdef ENABLE_BUTTONSET
    GHButtonSet m_buttonSet;
#endif // ENABLE_BUTTONSET
    GHAir m_air;
    int m_childEEPROMPointer;   // Current EEPROM Address of childs
    /*
    Relay m_relay1(pin_RELAY_1);
    Relay m_relay2(pin_RELAY_2);
    Relay m_relay3(pin_RELAY_3);
    Relay m_relay4(pin_RELAY_4);
    */
public:
    Manager();
    GHRTC &rtc() { return this->m_rtc; };
    GHDisplay &display() { return this->m_display; };
    GH1Wire &onewire() { return this->m_1wire; };
    GHRelay &relay1() { return this->m_relay1; };
    GHAir &air() { return this->m_air; };
    void setup();
    void loop();

};

#endif // __MANAGER_H__
