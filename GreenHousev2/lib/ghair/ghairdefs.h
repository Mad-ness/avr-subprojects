#ifndef __GHAIRDEFS_H__
#define __GHAIRDEFS_H__

#define AIR_ADDRESS_SIZE        6
#define AIR_MAX_PACKET_SIZE     32
#define AIR_MAX_DATA_SIZE       29

// Samples of commands are presented
// but you can define its own
#define AIR_ADDR_NULL                   0x0

#define AIR_CMD_UNDEF                   0X0
#define AIR_CMD_IN_PING                 0x1
#define AIR_CMD_OUT_PONG                0x2
#define AIR_CMD_IN_RESET                0x3
#define AIR_CMD_IN_GET_EEPROM           0x4
#define AIR_CMD_OUT_GET_EEPROM          0x5
#define AIR_CMD_IN_WRITE_EEPROM         0x6
#define AIR_CMD_OUT_WRITE_EEPROM_OK     0x7
#define AIR_CMD_OUT_WRITE_EEPROM_FAIL   0x8
#define AIR_CMD_IN_DATA                 0x9
#define AIR_CMD_OUT_DATA                0xA
#define AIR_CMD_IN_CMD1                 0xB
#define AIR_CMD_OUT_CMD1                0xC
#define AIR_CMD_IN_CMD2                 0xD
#define AIR_CMD_OUT_CMD2                0xE


/*
enum AirCommand {
    Undef = 0x0,
    Ping,
    Pong,
    Reset,
    Read,
    SendRead,
    Write,
    WriteAck,
    Reserv0,    // you can implement you own handlers for the ReservX commands
    Reserv1,
    Reserv2,
    Reserv3
};
*/
struct AirPacket {
    int8_t command;
    //AirCommand command;
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
        command = AIR_CMD_UNDEF; address = AIR_ADDR_NULL; length = 0x0;
        memset(&data, 0, AIR_MAX_DATA_SIZE);
    }
};


#endif // __GHAIRDEFS_H__
