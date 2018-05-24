#include <manager.h>

Manager::Manager()
: m_display(pin_DISP_SCLK, pin_DISP_SID, pin_DISP_DC, pin_DISP_CS, pin_DISP_RST, pin_DISP_BL),
  m_1wire(pin_1Wire), m_relay1(pin_Relay_1), m_buttonSet(pin_ButtonSet)
{


}

void Manager::setup() {
#ifdef DEBUG_SERIAL
    Serial.begin(57600);
#endif
    this->m_display.setup();
    this->m_1wire.setup();
    this->m_relay1.setup();
    //pinMode(13, OUTPUT); // temprorary use pin 13 as a power source
    //digitalWrite(13, HIGH);
    this->m_relay1.setEEPROMAddress(0x0);
    this->m_display.setEnableBL(true);
    this->m_display.setRTC(&this->m_rtc);
    this->m_display.set1Wire(&this->m_1wire);
    this->m_display.setRelay(&this->m_relay1, 0);
    this->m_relay1.setOperationalTime(19, 00, 150);
    this->m_display.displayMainPage();
    this->m_rtc.setup();
    this->m_display.setEnableBL(false);
}

void Manager::loop() {
    if (m_buttonSet.buttons().update()) {
        if (m_buttonSet.buttons().get() == 1) {
            if ( m_display.getBLState() == false ) {
                m_display.setEnableBL(true);
            } else {
                m_display.setEnableBL(false);
            }
        }
        this->m_display.showKeysPage(m_buttonSet.buttons().getRaw());
        delay(2000);
    }
    m_rtc.loop();
    m_1wire.loop();
    m_relay1.loop();
    if ( millis() % 2000 == 0) {
        this->m_display.displayMainPage();
    }
}
