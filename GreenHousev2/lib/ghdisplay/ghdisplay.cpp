#include <ghdisplay.h>
#include <ghrtc.h>
#include <gh1wire.h>
#include <ghrelay.h>


GHDisplay::GHDisplay(const int8_t SCLK, const int8_t DIN, const int8_t DC, const int8_t CS, const int8_t RST, const int8_t BL)
: Adafruit_PCD8544( SCLK, DIN, DC, CS, RST)
{
    this->m_BL_pin = BL;
    pinMode(BL, OUTPUT);
}

void GHDisplay::setup(void) {
    this->begin();
    this->setContrast(50);
};

void GHDisplay::setEnableBL(const bool enable) {
    digitalWrite( this->m_BL_pin, HIGH ? enable == true : LOW );
}

bool GHDisplay::getBLState() {
    return true ? digitalRead( this->m_BL_pin ) == HIGH : false;
}

void GHDisplay::showWelcomePage() {
    delay(500);
    this->clearDisplay();
    this->setTextSize(1);
    this->setCursor(0, 0);
    this->setTextColor(BLACK);

    char messages[3][10] = {"Loading\n", "program\n", "data"};
    for (int8_t i = 0; i < 3; i++) {
        int8_t k = 0;
        char c = messages[i][k];
        while (c != '\0') {
            this->print(c);
            this->display();
            delay(50);
            c = messages[i][++k];
        }
    }
    delay(500);
}

void GHDisplay::setRTC(GHRTC *rtc) {
    this->m_rtc = rtc;
}

void GHDisplay::set1Wire(GH1Wire *onewire) {
    this->m_1wire = onewire;
}

void GHDisplay::setRelay(GHRelay *relay, const int8_t index) {
    this->m_relays[index] = relay;
}


void GHDisplay::displayMainPage() {
    this->clearDisplay();
    this->setTextSize(1);
    char dt_str[18];
    if ( this->m_rtc->isAvailable() ) {
        GHRTCDateTime dt;
        this->m_rtc->getDateTime(&dt);
        sprintf(dt_str, "%02d/%02d %02d:%02d:%02d\n", dt.day, dt.month, dt.hour, dt.minute, dt.second);
    } else {
        *dt_str = "Clock is halted";
    }
    this->setCursor(0, 0);
    this->print(dt_str);
    this->drawFastHLine(0, 10, this->width(), BLACK);
    this->m_1wire->requestTemperatures(0);
    char temp_str[15];
    sprintf(temp_str, "Temp: %2.2f C", this->m_1wire->getTempCByIndex(0));
    this->setCursor(0, 12);
    this->print(temp_str);
    this->setCursor(0, 24);
    if ( this->m_relays[0]->state() == LOW ) {
        this->print("Relay 0 : OFF\n");
    } else {
        this->print("Relay 0 : RUN\n");
    }
    uint8_t hour, minute;
    uint16_t length;
    this->m_relays[0]->getOperationalTime(&hour, &minute, &length);
    char time_str[20];
    sprintf(time_str, "Optime: %02d:%02d + %d mins", hour, minute, length);
    this->print(time_str);
    this->display();
}

/*
GHLCDMenu::GHLCDMenu() {
    this->m_currentPage = 0;
}
*/
