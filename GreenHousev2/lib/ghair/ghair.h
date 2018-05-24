#ifndef __GH_AIR_H__
#define __GH_AIR_H__

#include <Arduino.h>
#include <RF24.h>
#include <ghairdefs.h>

class GHAir {
private:
    typedef void(*on_packet_handler_t)(AirPacket*);
    RF24 m_rf24;
    struct {
        byte read[AIR_ADDRESS_SIZE];
        byte write[AIR_ADDRESS_SIZE];
    } m_pipes;
    //byte m_address[AIR_ADDRESS_SIZE][2];
    on_packet_handler_t m_handler;
    AirPacket m_packet;
    void startListening();
    void stopListening();
public:
    GHAir(const int ce_pin, const int csn_pin, byte *read_pipe, byte *write_pipe);
    RF24 *rf24();
    void setup();
    void loop();
    bool hasData();
    // returns True if a packet is delivered
    bool sendPacket(const int8_t cmd, const int8_t addr, const int8_t len, void *data);
    // Air commands
    bool sendPing();
    bool sendPong();
    bool sendData(void *data, uint8_t len);
    void sendWriteEEPROM(const int8_t addr, const uint8_t value);
    uint8_t sendReadEEPROM(const int8_t addr);
    // void onGetData(void (*func)(AirPacket *packet));
    void onGetData(on_packet_handler_t handler);
    AirPacket &packet();
};

#endif // __GH_AIR_H__
