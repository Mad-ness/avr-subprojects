#include <manager.h>

#define EEPROM_BASEADDR_RELAY0      0x010
#define EEPROM_BASEADDR_RELAY1      0x020
#define EEPROM_BASEADDR_RELAY2      0x030
#define EEPROM_BASEADDR_RELAY3      0x040
#define EEPROM_BASEADDR_RELAY4      0x050
#define EEPROM_BASEADDR_1WIRE       0x060

Manager::Manager()
: m_display(pin_DISP_SCLK, pin_DISP_SID, pin_DISP_DC, pin_DISP_CS, pin_DISP_RST, pin_DISP_BL),
  m_1wire(pin_1Wire), m_relay1(RELAY_ID_0, pin_Relay_1),
#ifdef ENABLE_BUTTONSET
  m_buttonSet(pin_ButtonSet),
#endif // ENABLE_BUTTONSET
  m_air(pin_Air_CE, pin_Air_CSN, AirPeers[0], AirPeers[1])
{
    m_childEEPROMPointer = 0;
}


void Manager::setup() {
    //air = GHAir(pin_Air_CE, pin_Air_CSN, AirPeers[0], AirPeers[1]);
//    air.setHandler(&this->airHandler);
    this->m_display.setup();
    this->m_1wire.setup();
    this->m_relay1.setEEPROMBaseAddr(EEPROM_BASEADDR_RELAY0);
    this->m_relay1.setup();
    //pinMode(13, OUTPUT); // temprorary use pin 13 as a power source
    //digitalWrite(13, HIGH);
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
    m_air.loop();
#ifdef ENABLE_BUTTONSET
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
#endif // ENABLE_BUTTONSET
    m_rtc.loop();
    m_1wire.loop();
    m_relay1.loop();
    if ( millis() % 2000 == 0) {
        this->m_display.displayMainPage();
    }
}
