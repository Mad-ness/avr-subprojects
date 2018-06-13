#ifdef EEPROM_SUPPORT
#include <EEPROM.h>
#endif
#include <ghobject.h>


void GHObject::setup() {
}

void GHObject::loop() {

}

bool GHObjectEx::setEEPROM(int rel_address, uint8_t value) {
    bool result = false;
#ifdef EEPROM_SUPPORT
    EEPROM.update(m_address + rel_address, value);
    if ( EEPROM.read(m_address + rel_address) == value ) {
        result = true;
    }
#endif
    return result;
}

uint8_t GHObjectEx::getEEPROM(int rel_address) {
#ifdef EEPROM_SUPPORT
    return EEPROM.read(m_address + rel_address);
#else
    return 0;
#endif
}

void GHObjectEx::setEEPROMBaseAddr(int address) {
    this->m_address = address;
}

int GHObjectEx::getEEPROMBaseAddr() {
    return this->m_address;
}
