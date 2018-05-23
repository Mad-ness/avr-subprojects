#ifndef __GH_AIR_H__
#define __GH_AIR_H__

#include <Arduino.h>
#include <RF24.h>
#include <ghairdefs.h>

class GHAir {
private:
    RF24 m_rf24;
    byte m_address[AIR_ADDRESS_SIZE];
    bool m_hasData;
    void writeEEPROM(const int8_t addr, const uint8_t value);
    uint8_t readEEPROM(const int8_t addr);
    AirPacket m_packet;
public:
    GHAir(const int ce_pin, const int csn_pin, const uint8_t *address);
    RF24 *rf24();
    void setup();
    void loop();
    void startListening();
    void stopListening();
    // after calling this function, next call returns false;
    bool hasData();
    bool sendPacket(const int8_t cmd, const int8_t addr, const int8_t len, void *data);
    // Air commands
    bool cmdPing();
    void cmdPong();
    AirPacket &packet();
};

#endif // __GH_AIR_H__
