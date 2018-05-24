#include <EEPROM.h>
#include <ghair.h>


GHAir::GHAir(const int ce_pin, const int csn_pin, byte *read_pipe, byte *write_pipe)
: m_rf24(ce_pin, csn_pin)
{
    //this->m_pipes.read[0] = read_pipe;
    //this->m_pipes.write[0] = write_pipe;
    memcpy(this->m_pipes.read, read_pipe, AIR_ADDRESS_SIZE);
    memcpy(this->m_pipes.write, write_pipe, AIR_ADDRESS_SIZE);
}

RF24 *GHAir::rf24() {
    return &this->m_rf24;
}

void GHAir::setup() {
    this->m_rf24.begin();
    this->m_rf24.openReadingPipe(1, this->m_pipes.read);
    this->m_rf24.openWritingPipe(this->m_pipes.write);
    this->startListening();
}

inline void GHAir::startListening() {
    this->m_rf24.startListening();
}

inline void GHAir::stopListening() {
    this->m_rf24.stopListening();
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

#ifdef DEBUG_AIR
    Serial.println("  >>> Prepare a package for sending");
    char info[40];
    sprintf(info, "  >>> Packet length is %d (byes), Msg: %s", pkt.length, pkt.data);
    Serial.println(info);
    Serial.println();
#endif

    memcpy(&pkt.data, data, pkt.length);

#ifdef DEBUG_AIR
    Serial.println("  >> Copied data to the packet buffer");
#endif

    this->stopListening();

#ifdef DEBUG_AIR
    Serial.println("  >> Stopped to listening to the air");
#endif

    bool result = this->m_rf24.write(&pkt, sizeof(pkt));

#ifdef DEBUG_AIR
    if ( result ) {
        Serial.println("  >>> Packet sending OK");
    } else {
        Serial.println("  >>> Packet sending FAIL");
    }
#endif
    this->startListening();
    return result;
}

bool GHAir::hasData() {
    return this->m_rf24.available();
}

void GHAir::writeEEPROM(const int8_t addr, uint8_t value) {
    EEPROM.write(addr, value);
}

uint8_t GHAir::readEEPROM(const int8_t addr) {
    return EEPROM.read(addr);
}

bool GHAir::cmdPong() {
    char greenhouse[] = "Pong:GreenHouse";
    return this->sendPacket(AIR_CMD_PONG, 0x0, strlen(greenhouse), &greenhouse);
}

bool GHAir::cmdPing() {
    char msg[] = "Hello, bro!";
    return this->sendPacket(AIR_CMD_PING, 0x0, strlen(msg), &msg);
}

/*
void GHAir::onGetData(void (*func)(AirPacket *packet)) {
    func(&this->packet());
}
*/
void GHAir::onGetData(on_packet_handler_t handler) {
    this->m_handler = handler;
}

void GHAir::loop() {
    if ( this->m_rf24.available() && this->m_handler != NULL ) {
        this->m_rf24.read(&this->m_packet, sizeof(this->m_packet));
        this->m_handler(&this->m_packet);
    }
}
