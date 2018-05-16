#include <ghrelay.h>
#include <ghrtc.h>

GHRelayBase::GHRelayBase(const int8_t pin) {
    this->m_pin = pin;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    this->m_workStarted = false;
    this->m_workStateFlag = false;
    this->onEnd();
};

void GHRelayBase::setEEPROMAddress(const int addr) {
}

bool GHRelayBase::state() {
    return this->m_workStarted;
}
void GHRelayBase::setRTC(GHRTC *rtc) {
    this->m_rtc = rtc;
}

void GHRelayBase::onBegin() {
    if ( this->m_workStateFlag ) {
        digitalWrite(this->m_pin, HIGH);
    } else {
        digitalWrite(this->m_pin, LOW);
    }
#ifdef DEBUG_SERIAL_RELAY
    Serial.println("Work Started");
#endif
}
void GHRelayBase::onEnd() {
    if ( this->m_workStateFlag ) {
        digitalWrite(this->m_pin, LOW);
    } else {
        digitalWrite(this->m_pin, HIGH);
    }
#ifdef DEBUG_SERIAL_RELAY
        Serial.println("Work Ended");
#endif
}

void GHRelayBase::loop() {
    // Make sure that it will not work too frequently
    if (abs( millis() - this->m_lastCheck ) < 10*1000 ) return;
    this->m_lastCheck = millis();

#ifdef DEBUG_SERIAL_RELAY
    char str[20];
    sprintf(str, "Last check was %d", this->m_lastCheck);
    Serial.println(str);
#endif

    GHRTCDateTime dt;
    this->m_rtc->getDateTime(&dt);
    uint16_t now_minutes = dt.hour * 60 + dt.minute;
    uint16_t since_start_minutes = this->m_startTime.hour * 60 + this->m_startTime.minute;
    uint16_t since_stop_minutes = this->m_stopTime.hour * 60 + this->m_stopTime.minute;
#ifdef DEBUG_SERIAL_RELAY
    char str2[40];
    sprintf(str2, "Start: %d, Stop: %d, Now: %d", since_start_minutes, since_stop_minutes, now_minutes);
    Serial.println(str2);
#endif

    if ( since_start_minutes < since_stop_minutes ) {
        bool after_stoptime = ( now_minutes >= since_stop_minutes );
        bool after_starttime = ( now_minutes >= since_start_minutes );

        if ( this->m_workStarted == false ) {
            if ( after_starttime && ( now_minutes < since_stop_minutes )) {
                this->m_workStarted = true;
                this->onBegin();
            }
        } else {
            if ( after_stoptime && ( since_start_minutes - now_minutes ) >= 0 ) {
                this->m_workStarted = false;
                this->onEnd();
            }
        }

    } else {
        if ( this->m_workStarted == false ) {
            if ( now_minutes < since_stop_minutes ) {
                this->m_workStarted = true;
                this->onBegin();
            }
        } else {
            if ( now_minutes >= since_stop_minutes ) {
                this->m_workStarted = false;
                this->onEnd();
            }
        }
    }
};

const int *GHRelayBase::getLength() {
    return &this->m_lenght;
}

void GHRelayBase::workStateIsHIGH(const bool flag) {
    this->m_workStateFlag = flag;
}

GHRelayBase::Time *GHRelayBase::getStartTime() {
    return &this-> m_startTime;
}

void GHRelayBase::getOperationalTime(uint8_t *hour, uint8_t *minute, uint16_t *length) {
    *hour = this->m_startTime.hour;
    *minute = this->m_startTime.minute;
    *length = this->m_lenght;
}

void GHRelayBase::setOperationalTime(const uint8_t hour, const uint8_t minute, const uint16_t minutes) {
    this->m_startTime = { hour, minute };
    this->m_lenght = minutes;
    uint16_t startminutes = hour * 60 + minute;
    uint16_t stopminutes = startminutes + minutes;

    if ( stopminutes > 24 * 60 ) {
        stopminutes = stopminutes - 24 * 60;
    }
    this->m_stopTime.hour = stopminutes / 60;
    this->m_stopTime.minute = stopminutes - this->m_stopTime.hour*60;
}
// --------- *** GHRelay class *** -------- //
GHRelay::GHRelay(const int8_t pin)
: GHRelayBase(pin)
{

}
