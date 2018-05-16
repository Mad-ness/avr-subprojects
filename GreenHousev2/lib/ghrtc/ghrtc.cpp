#include <ghrtc.h>

GHRTC::GHRTC()
//: Ds1307(0x68)
{
}

void GHRTC::setup() {
    this->m_rtc.start();
}

void GHRTC::loop() {

}

DS1307* GHRTC::rtc(void) {
     return &this->m_rtc;
}

bool GHRTC::isAvailable() {
    return true;
}

void GHRTC::getDateTime(GHRTCDateTime *dt) {
    this->m_rtc.get(&(dt->second), &(dt->minute), &(dt->hour), &(dt->day), &(dt->month), &(dt->year));
#ifdef DEBUG_SERIAL_RTC
    const char str[48];
    sprintf(str, "Time from RTC: %02d:%02d:%02d %02d/%02d/%04d", dt->hour, dt->minute, dt->second,
                                                                     dt->month, dt->day, dt->year);
    Serial.println(str);
#endif
}
