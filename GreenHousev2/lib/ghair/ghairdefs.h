#ifndef __GHAIRDEFS_H__
#define __GHAIRDEFS_H__

#define AIR_ADDRESS_SIZE        6
#define AIR_MAX_PACKET_SIZE     32
#define AIR_MAX_DATA_SIZE       29

#define AIR_ADDR_NULL           0x0
#define AIR_CMD_UNDEF           0X0
#define AIR_CMD_PING            0x1
#define AIR_CMD_PONG            0x2
#define AIR_CMD_SET             0x3
#define AIR_CMD_GET             0x4
#define AIR_CMD_RESET           0x5
#define AIR_CMD_READ_EEPROM     0x6
#define AIR_CMD_WRITE_EEPROM    0x7
#define AIR_CMD_DATA            0x8

typedef struct AirPacket {
    int8_t command;
    int8_t address;
    int8_t length;      // length of the data
    byte data[AIR_MAX_DATA_SIZE];
    // size of a entire packet
    uint8_t size() {
        return sizeof(command) + sizeof(address) + sizeof(length) + length;
    }
    uint8_t size_all() {
        return sizeof(this);
    }
    void flush() {
        command = 0x0; address = 0x0; length = 0x0;
        memset(&data, 0, AIR_MAX_DATA_SIZE);
    }
};

#endif // __GHAIRDEFS_H__
