#ifndef __GH_AIR_H__
#define __GH_AIR_H__

#include <Arduino.h>
#include <RF24.h>

#define AIR_ADDRESS_SIZE        6
#define AIR_MAX_PACKET_SIZE     32
#define AIR_MAX_DATA_SIZE       29

#define AIR_CMD_PING            0x1
#define AIR_CMD_PONG            0x2
#define AIR_CMD_SET             0x3
#define AIR_CMD_GET             0x4
#define AIR_CMD_RESET           0x5
#define AIR_CMD_READ_EEPROM     0x6
#define AIR_CMD_WRITE_EEPROM    0x7

typedef struct AirPacket {
    int8_t command;
    int8_t address;
    int8_t length;
    byte data[AIR_MAX_DATA_SIZE];
    uint8_t size() {
        return sizeof(command) + sizeof(address) + length;
    }
};

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
