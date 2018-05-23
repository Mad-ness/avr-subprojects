#include <EEPROM.h>
#include <ghair.h>


GHAir::GHAir(const int ce_pin, const int csn_pin, const uint8_t *address)
: m_rf24(ce_pin, csn_pin)
{
    memcpy(&this->m_address, address, AIR_ADDRESS_SIZE);
    this->m_hasData = false;
}

RF24 *GHAir::rf24() {
    return &this->m_rf24;
}

void GHAir::setup() {
    this->m_rf24.begin();
    this->startListening();
}

void GHAir::startListening() {
    this->m_rf24.openReadingPipe(0, &this->m_address);
    this->m_rf24.startListening();
}

void GHAir::stopListening() {
    this->m_rf24.stopListening();
    this->m_rf24.openWritingPipe(this->m_address);
}

AirPacket &GHAir::packet() {
    return this->m_packet;
}

bool GHAir::sendPacket(const int8_t cmd, const int8_t addr, const int8_t len, void *data) {
    AirPacket pkt;
    pkt.command = cmd;
    pkt.address = addr;
    pkt.length = len;
    if ( len > AIR_MAX_DATA_SIZE )
        pkt.length = AIR_MAX_DATA_SIZE;
    memcpy(&pkt.data, data, pkt.length);
    this->stopListening();
    return this->m_rf24.write(&pkt, pkt.length);
}

void GHAir::loop() {
    if ( this->m_rf24.available() ) {
        memset(&this->m_packet, 0, sizeof(this->m_packet));
        this->m_rf24.read(&this->m_packet, sizeof(this->m_packet));
        this->m_hasData = true;

        int8_t cmd = this->m_packet.command;
        int8_t address = this->m_packet.address;
        int8_t length = this->m_packet.length;
        void *data = &this->m_packet.data;

        switch ( cmd ) {
            case AIR_CMD_PING: this->cmdPong(); break;
            case AIR_CMD_PONG: break;
            // writing byte(s) in EEPROM starting from the specified address
            case AIR_CMD_WRITE_EEPROM:
                for (int i = 0; i < length; i++) {
                    this->writeEEPROM(address+i, (uint8_t)(data+i));
                }
                break;
            case AIR_CMD_READ_EEPROM:
                for (int i = 0; i < length; i++) {

                }
                break;
        }
    }
}

bool GHAir::hasData() {
    bool result = this->m_hasData;
    if ( result ) this->m_hasData = false;
    return result;
}

void GHAir::writeEEPROM(const int8_t addr, uint8_t value) {
    EEPROM.write(addr, value);
}

uint8_t GHAir::readEEPROM(const int8_t addr) {
    return EEPROM.read(addr);
}

void GHAir::cmdPong() {
    char greenhouse[] = "Pong:GreenHouse\0";
    this->sendPacket(AIR_CMD_PONG, 0x0, sizeof(greenhouse), &greenhouse);
}

bool GHAir::cmdPing() {
    return this->sendPacket(AIR_CMD_PING, 0x0, 0x0, 0x0);
}
