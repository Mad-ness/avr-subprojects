#ifdef ARDUINO_ARCH_AVR
#include <EEPROM.h>
#include <avr/wdt.h>
#endif // ARDUINO
#include <ghair.h>
#if defined(__LINUX__)
    #include <time.h>
#endif // __LINUX__

// void (*resetBoardVector)(void) = 0;
#ifdef DEBUG_AIR
uint16_t cycles_cnt = 0;
#endif

#ifdef ARDUINO_ARCH_AVR
void resetBoardVector() {
    wdt_enable(WDTO_15MS);
    while (1); // wait until the reset
}
#endif // ARDUINO

GHAir::GHAir(const int ce_pin, const int csn_pin, byte *read_pipe, byte *write_pipe)
#if defined(__LINUX__)
: m_rf24(ce_pin, csn_pin, "/dev/spidev0.0")
#else 
: m_rf24(ce_pin, csn_pin)
#endif
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
#if defined(ARDUINO)
    this->m_rf24.openReadingPipe(1, this->m_pipes.read);
    this->m_rf24.openWritingPipe(this->m_pipes.write);
#elif defined(__LINUX__)
    uint64_t pipes[2];
    memcpy(&pipes[0], this->m_pipes.read, sizeof(this->m_pipes.read));
    memcpy(&pipes[1], this->m_pipes.write, sizeof(this->m_pipes.write));
    this->m_rf24.openReadingPipe(1, pipes[0]);
    this->m_rf24.openWritingPipe(pipes[1]);
#endif
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

bool GHAir::sendPacket(const AirPacket &pkt) {
    return sendPacket(pkt.command, pkt.request_id, pkt.address, pkt.length, &pkt.data);
}

bool GHAir::sendPacket(const uint8_t cmd, const uint8_t packet_id, const uint8_t addr, const uint8_t len, const void *data) {
    if ( ! m_rf24.isChipConnected() ) return false;
    AirPacket pkt;
    pkt.command = cmd;
    pkt.address = addr;
    pkt.length = len;
    pkt.request_id = packet_id;
    if ( len > AIR_MAX_DATA_SIZE )
        pkt.length = AIR_MAX_DATA_SIZE;

#ifdef DEBUG_AIR_TRACE2
    char info[50];
    sprintf(info, ">>> SENDing Cmd: 0x%02x, Addr: 0x%02x, Id: %d, Length %d (bytes)\0", cmd, addr, packet_id, pkt.length);
    Serial.println(info);
    if ( pkt.isResponse() && pkt.getCommand() == AIR_CMD_IN_UPTIME ) {
        unsigned long uptime;
        memcpy(&uptime, pkt.data, sizeof(uptime));
        sprintf(info, "Value of pkt.data in the sendPacket = %lu\n", uptime);
        Serial.print(info);
    }
#endif
    if ( pkt.length > 0 ) {
        memmove(&pkt.data, data, pkt.length);
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

#ifdef DEBUG_AIR_TRACE
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
    return this->sendResponse(this->m_packet, true, this->m_packet.length, &this->m_packet.data);
}

bool GHAir::sendPing() {
    unsigned long uptime = millis();
    return this->sendPacket(AIR_CMD_IN_PING, packet_id, 0x0, sizeof(uptime), &uptime);
}

void GHAir::setHandler(on_packet_handler_t handler) {
    this->m_handler = handler;
}


bool GHAir::sendData(void *data, uint8_t len) {
    return this->sendPacket(AIR_CMD_IN_PING, packet_id, AIR_ADDR_NULL, len, data);
}

bool GHAir::sendUptime() {
    return this->sendPacket(AIR_CMD_IN_UPTIME, packet_id, AIR_ADDR_NULL, 0x0, NULL);
}

bool GHAir::sendResetBoard() {
    return this->sendPacket(AIR_CMD_IN_RESET, packet_id, AIR_ADDR_NULL, 0x0, NULL);
}

/**
 * Checks the buffer of the connected radio module on new data.
 * If calls GHAir::onGetDataStandard() method along with
 * the callback (if set) configured by calling the 
 * GHAir::setHandler(...).
 * ---
 * Call this in a loop as often as possible
 * on the client side (slave).
 */
void GHAir::loop() {
    if ( this->m_rf24.available() ) {
        this->m_rf24.read(&this->m_packet, sizeof(this->m_packet));
        this->onGetDataStandard();
    }
}

/**
 * Call this function in cycle as often as possible.
 * It checks the buffer of the connected radio module
 * for new data. 
 * It returns true if processing is required.
 * It returns false either no data came or they was processed by the 
 * onGetDataStandard() call.
 * New data available as the GHAir::packet();
 * ---
 * This function more suitable to run on the server side (initiator, master).
 */
bool GHAir::receivedPacket() {
    bool result = false;
    if ( this->m_rf24.available() ) {
        this->m_rf24.read(&this->m_packet, sizeof(this->m_packet));
        result = !this->onGetDataStandard();
    }
    return result;
}

bool GHAir::onGetDataStandard() {
    bool result = true;
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
#if defined(__LINUX__)
            case AIR_CMD_IN_GETTIME: {
                time_t t = time(NULL);
                sendResponse(pkt, true, sizeof(t), &t);
                }; break;
#endif // __LINUX__
            case AIR_CMD_IN_PING:
                this->sendPong();
                break;
#ifdef ARDUINO_ARCH_AVR
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
#endif // ARDUINO
#ifdef DEBUG_AIR
                sprintf(str, "  == Current milliseconds (local) %lu\n", uptime);
                Serial.print(str);
#endif // DEBUG_AIR
#ifdef ARDUINO_ARCH_AVR
                }
                break;
            case AIR_CMD_SET_PIN_MODE: {
                    pinMode(pkt.address, (const uint8_t*)pkt.data);
                    this->sendResponse(pkt, true, 0x0, NULL);
                }
                break;
            case AIR_CMD_SET_PIN_VALUE: {
                    digitalWrite(pkt.address, (const uint8_t*)pkt.data);
                    this->sendResponse(pkt, true, 0x0, NULL);
                }
                break;
            case AIR_CMD_GET_PIN_VALUE: {
                    const uint8_t value = digitalRead(pkt.address);
                    this->sendResponse(pkt, true, 0x1, &value);
                }
                break;
            case AIR_CMD_SET_PWM_VALUE: {
                    uint16_t value;
                    memcpy(&value, &pkt.data, 2);
                    analogWrite(pkt.address, &value);
                    this->sendResponse(pkt, true, 0x2, &value);
                }
                break;
            case AIR_CMD_GET_PWM_VALUE: {
                    uint16_t value = analogRead(pkt.address);
                    this->sendResponse(pkt, true, 0x2, &value);
                }
                break;

#endif // ARDUINO
            default:
                if ( this->m_handler != NULL ) {
                    this->m_handler(&this->m_packet);
                }
                result = false;
                break;
        }
    }
    return result;
}

#ifdef ARDUINO_ARCH_AVR

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

bool GHAir::onReadEEPROM(uint8_t address) {
    int8_t buf = EEPROM.read(address);
    return this->sendResponse(this->m_packet, true, sizeof(buf), &buf);
}
/*
void GHAir::onSetPinAsInput(const uint8_t pin) {
    pinMode(pin, INPUT);
    this->sendResponse(this->m_packet, true, m_packet.length, &m_packet.data);
};

void GHAir::onSetPinAsOutput(const uint8_t pin) {
    pinMode(pin, OUTPUT);
    this->sendResponse(this->m_packet, true, m_packet.length, &m_packet.data);
};
*/

#endif // ARDUINO

bool GHAir::sendWriteEEPROM(uint8_t address, int8_t value) {
    return this->sendPacket(AIR_CMD_IN_WRITE_EEPROM, packet_id, address, sizeof(value), &value);
}

bool GHAir::sendReadEEPROM(uint8_t address) {
    return this->sendPacket(AIR_CMD_IN_GET_EEPROM, packet_id, address, 0x0, NULL);
}

bool GHAir::sendSetPinHigh(const uint8_t pin) {
    uint8_t value = HIGH;
    return this->sendPacket(AIR_CMD_SET_PIN_VALUE, packet_id, pin, 0x1, &value);
}

bool GHAir::sendSetPinLow(const uint8_t pin) {
    uint8_t value = LOW;
    return this->sendPacket(AIR_CMD_SET_PIN_VALUE, packet_id, pin, 0x1, &value);
}

bool GHAir::sendGetPinValue(const uint8_t pin) {
    return this->sendPacket(AIR_CMD_GET_PIN_VALUE, packet_id, pin, 0x0, NULL);
}

bool GHAir::sendSetPWMValue(const uint8_t pin, const uint8_t value) {
    return this->sendPacket(AIR_CMD_SET_PWM_VALUE, packet_id, pin, 0x1, (const void*)&value);
}

bool GHAir::sendGetPWMValue(const uint8_t pin) {
    return this->sendPacket(AIR_CMD_GET_PWM_VALUE, packet_id, pin, 0x0, NULL);
}

bool GHAir::sendSetPinAsInput(const uint8_t pin) {
    uint8_t value = INPUT;
    return this->sendPacket( AIR_CMD_SET_PIN_MODE, packet_id, pin, 0x1, &value);
}

bool GHAir::sendSetPinAsOutput(const uint8_t pin) {
    uint8_t value = OUTPUT;
    return this->sendPacket( AIR_CMD_SET_PIN_MODE, packet_id, pin, 0x1, &value );
}





/**
  in_pkt - packet the response on which will be sent
  resp_ok_or_fail - was the command executed successfully or not data* - a pointer to the data is being send in the AirPacket.data buffer
  datalen - number of bytes in the data
 */
bool GHAir::sendResponse(const AirPacket &in_pkt, bool resp_ok_or_fail, uint8_t datalen, void *data) {
    AirPacket pkt(in_pkt);
    pkt.markAsResponse(resp_ok_or_fail);
#ifdef DEBUG_AIR
    char str[80];
    sprintf(str, "     Sending RESPONSE onfunc:%02x, good:%d, datalen:%d (bytes)\n", pkt.getCommand(), pkt.isGoodResponse(), datalen);
    Serial.print(str);
#endif // DEBUG_AIR
    return this->sendPacket(pkt.command, pkt.request_id, pkt.address, datalen, data);
}

void 
GHAir::setPacketId(const uint8_t id) {
    m_packet.request_id = id;
    packet_id = id;
}


#if defined(__LINUX__)
void 
GHAir::onGetLocalTime() {
    
}
#endif // __LINUX__

