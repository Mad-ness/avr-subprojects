#ifndef __GHDISPLAY_H__
#define __GHDISPLAY_H__

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

class GH1Wire;
class GHRTC;
class GHRelay;

/*

#define DISP_SID_pin  5 // Serial Data In (Din)
#define DISP_SLCK_pin 6 // SCLK, clock (SCK, CLK)
#define DISP_DC_pin   4 // D/C, Data/Command Selector
#define DISP_RST_pin  2 //
#define DISP_CS_pin   3 // CS, Chip Select
#define DISP_BL_pin   7

*/

class GHDisplay: public Adafruit_PCD8544 {
private:
    int8_t m_BL_pin;
    GHRTC *m_rtc;
    GH1Wire *m_1wire;
    GHRelay *m_relays[1];
public:
    GHDisplay(const int8_t SCLK, const int8_t DIN, const int8_t DC, const int8_t CS, const int8_t RST, const int8_t BL);
    void displayMainPage();
    void setEnableBL(const bool enable);
    bool getBLState();
    void setup(void);
    void setRTC(GHRTC *rtc);
    void set1Wire(GH1Wire *onewire);
    void setRelay(GHRelay *relay, const int8_t index);
    void showWelcomePage();
};

#endif // __GHDISPLAY_H__
