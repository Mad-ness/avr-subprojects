#ifndef __GH_AIR_H__
#define __GH_AIR_H__

#include <Arduino.h>
#include <RF24.h>
#include <ghairdefs.h>

class GHAir {
public:
    typedef void(*on_packet_handler_t)(AirPacket*);
private:
    RF24 m_rf24;
    struct {
        byte read[AIR_ADDRESS_SIZE];
        byte write[AIR_ADDRESS_SIZE];
    } m_pipes;
    uint8_t m_numAttempts;
    //byte m_address[AIR_ADDRESS_SIZE][2];
    on_packet_handler_t m_handler;
    AirPacket m_packet;
    void startListening();
    void stopListening();
    void onReceiveCmd();
    // handle requests
    bool onWriteEEPROM(uint8_t address, int8_t value);
    bool onReadEEPROM(uint8_t address);
    void onGetDataStandard();
    void onResetBoard();
public:
    GHAir(const int ce_pin, const int csn_pin, byte *read_pipe, byte *write_pipe);
    RF24 *rf24();
    AirPacket &packet();
    void setHandler(on_packet_handler_t handler);
    void setup();
    void loop();
    void setNumAttemps(uint8_t);
    uint8_t getNumAttempts();
    // returns True if a packet is delivered
    bool sendPacket(const uint8_t cmd, const uint8_t addr, const uint8_t len, void *data);
    // Air commands
    bool sendPing();
    bool sendPong();
    bool sendUptime();
    bool sendResetBoard();
    bool sendData(void *data, uint8_t len);
    bool sendResponse(const AirPacket &in_pkt, bool resp_ok_or_fail, uint8_t datalen, void *data);
    // responses to requests
    bool sendWriteEEPROM(uint8_t address, int8_t value);
    bool sendReadEEPROM(uint8_t address);
};

#endif // __GH_AIR_H__
