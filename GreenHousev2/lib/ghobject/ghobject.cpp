#include <EEPROM.h>
#include <ghobject.h>


void GHObject::setup() {
}

void GHObject::loop() {

}

bool GHObjectEx::setEEPROM(int rel_address, uint8_t value) {
    bool result = false;
    EEPROM.update(m_address + rel_address, value);
    if ( EEPROM.read(m_address + rel_address) == value ) {
        result = true;
    }
    return result;
}

uint8_t GHObjectEx::getEEPROM(int rel_address) {
    return EEPROM.read(m_address + rel_address);
}

void GHObjectEx::setEEPROMBaseAddr(int address) {
    this->m_address = address;
}

int GHObjectEx::getEEPROMBaseAddr() {
    return this->m_address;
}
