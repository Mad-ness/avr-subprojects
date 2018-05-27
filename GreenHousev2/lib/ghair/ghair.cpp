#include <EEPROM.h>
#include <avr/wdt.h>
#include <ghair.h>

// void (*resetBoardVector)(void) = 0;
#ifdef DEBUG_AIR
uint16_t cycles_cnt = 0;
#endif

void resetBoardVector() {
    wdt_enable(WDTO_15MS);
    while (1); // wait until the reset
}

GHAir::GHAir(const int ce_pin, const int csn_pin, byte *read_pipe, byte *write_pipe)
: m_rf24(ce_pin, csn_pin)
{
    memcpy(&this->m_pipes.read, read_pipe, AIR_ADDRESS_SIZE);
    memcpy(&this->m_pipes.write, write_pipe, AIR_ADDRESS_SIZE);
}

RF24 *GHAir::rf24() {
    return &this->m_rf24;
}

void GHAir::setup() {
    this->setNumAttemps(10);
    this->m_rf24.begin();
    this->m_rf24.setPALevel(RF24_PA_LOW);
    this->m_rf24.setDataRate(RF24_250KBPS);
    this->m_rf24.openReadingPipe(1, this->m_pipes.read);
    this->m_rf24.openWritingPipe(this->m_pipes.write);
    this->m_rf24.startListening();
}

void GHAir::setNumAttemps(uint8_t num) {
    this->m_numAttempts = num;
}

uint8_t GHAir::getNumAttempts() {
    return this->m_numAttempts;
}

void GHAir::startListening() {
    this->m_rf24.startListening();
}

void GHAir::stopListening() {
    this->m_rf24.stopListening();
}

AirPacket &GHAir::packet() {
    return this->m_packet;
}

bool GHAir::sendPacket(const uint8_t cmd, const uint8_t addr, const uint8_t len, void *data) {
    AirPacket pkt;
    pkt.command = cmd;
    pkt.address = addr;
    pkt.length = len;
    if ( len > AIR_MAX_DATA_SIZE )
        pkt.length = AIR_MAX_DATA_SIZE;

#ifdef DEBUG_AIR_TRACE
    char info[40];
    sprintf(info, ">>> SENDing Cmd: 0x%02x, Addr: 0x%02x, Length %d (bytes)\0", cmd, addr, pkt.length);
    Serial.println(info);
#endif
    if ( pkt.length > 0 ) {
        memcpy(&pkt.data, data, pkt.length);
    }

#ifdef DEBUG_AIR_TRACE
//    Serial.println("  >> Copied data to the packet buffer");
#endif

    this->stopListening();

#ifdef DEBUG_AIR_TRACE
//    Serial.println("  >> Stopped listen to the air");
#endif

    uint8_t attempts = 0;
    bool result = false;
    while ( attempts++ < this->m_numAttempts ) {
        if ( this->m_rf24.write(&pkt, sizeof(pkt)) ) {
            result = true;
            break;
        }
#ifdef DEBUG_AIR_TRACE
        Serial.print("  >>>> Used "); Serial.print(attempts);
        Serial.println(" attempts.");
#endif
    }

#ifdef DEBUG_ADEBUG_AIR_TRACEIR
    if ( result ) {
        Serial.println("  >>> Packet sending OK");
    } else {
        Serial.println("  >>> Packet sending FAIL");
    }
#endif
    this->startListening();
    return result;
}

bool GHAir::sendPong() {
    char greenhouse[] = "Pong:GreenHouse\0";
    // return this->sendPacket(AIR_CMD_OUT_PONG, 0x0, sizeof(greenhouse)+1, greenhouse);
    return this->sendResponse(this->m_packet, true, sizeof(greenhouse)+1, greenhouse);
}

bool GHAir::sendPing() {
    char msg[] = "Hello, bro\0";
    return this->sendPacket(AIR_CMD_IN_PING, 0x0, sizeof(msg)+1, msg);
}

void GHAir::setHandler(on_packet_handler_t handler) {
    this->m_handler = handler;
}

void GHAir::onGetDataStandard() {
    if ( 1 ) {
        const AirPacket &pkt = this->m_packet;
#ifdef DEBUG_AIR
        char str[80];
        sprintf(str, "%03d. InCommand 0x%02x, Address 0x%02x, Datalen: %02d (bytes)\n", cycles_cnt++, pkt.command, pkt.address, pkt.length);
        Serial.print(str);
#endif // DEBUG_AIR
        // Handle only initiating commands (not responses)
        // pkt.command has to have bits 6 and 7 set to zero.
        switch ( pkt.command ) {
            case AIR_CMD_IN_PING:
                this->sendPong();
                break;
            case AIR_CMD_IN_RESET:
                this->onResetBoard();
                break;
            case AIR_CMD_IN_GET_EEPROM:
                this->onReadEEPROM(pkt.address);
                break;
            case AIR_CMD_IN_WRITE_EEPROM:
                this->onWriteEEPROM(pkt.address, pkt.data[0]);
                break;
            case AIR_CMD_IN_UPTIME: {
                    unsigned long uptime = millis();
                    this->sendResponse(pkt, true, sizeof(uptime), &uptime);
                }
                break;
            default:
                if ( this->m_handler != NULL ) {
                    this->m_handler(&this->m_packet);
                }
                break;
        }
    }
}

void GHAir::loop() {
    if ( this->m_rf24.available() ) {
        this->m_rf24.read(&this->m_packet, sizeof(this->m_packet));
        this->onGetDataStandard();
    }
}

bool GHAir::sendData(void *data, uint8_t len) {
    return this->sendPacket(AIR_CMD_IN_PING, AIR_ADDR_NULL, len, data);
}

bool GHAir::sendUptime() {
    return this->sendPacket(AIR_CMD_IN_UPTIME, AIR_ADDR_NULL, 0x0, NULL);
}

bool GHAir::sendResetBoard() {
    return this->sendPacket(AIR_CMD_IN_RESET, AIR_ADDR_NULL, 0x0, NULL);
}

void GHAir::onResetBoard() {
    this->sendResponse(this->m_packet, true, 0x0, NULL);
    resetBoardVector();
}

bool GHAir::onWriteEEPROM(uint8_t address, int8_t value) {
    bool result = false;
    EEPROM.update(address, value);
    if ( EEPROM.read(address) == value ) {
        result = true;
    }
    return this->sendResponse(this->m_packet, result, 0x0, NULL);
}

bool GHAir::sendWriteEEPROM(uint8_t address, int8_t value) {
    return this->sendPacket(AIR_CMD_IN_WRITE_EEPROM, address, sizeof(value), &value);
}

bool GHAir::sendReadEEPROM(uint8_t address) {
    return this->sendPacket(AIR_CMD_IN_GET_EEPROM, address, 0x0, NULL);
}

bool GHAir::onReadEEPROM(uint8_t address) {
    int8_t buf = EEPROM.read(address);
    return this->sendResponse(this->m_packet, true, sizeof(buf), &buf);
}

/**
  in_pkt - packet the response on which will be sent
  resp_ok_or_fail - was the command executed successfully or not
  data* - a pointer to the data is being send in the AirPacket.data buffer
  datalen - number of bytes in the data
 */
bool GHAir::sendResponse(const AirPacket &in_pkt, bool resp_ok_or_fail, uint8_t datalen, void *data) {
    AirPacket pkt(in_pkt);
    pkt.markAsResponse(resp_ok_or_fail);
#ifdef DEBUG_AIR
    char str[80];
    sprintf(str, "     RESPONSE onfunc:%02x, good:%d, datalen:%d (bytes)\n", pkt.getCommand(), pkt.isGoodResponse(), in_pkt.length);
    Serial.print(str);
#endif // DEBUG_AIR
    return this->sendPacket(pkt.command, pkt.address, datalen, data);
}
