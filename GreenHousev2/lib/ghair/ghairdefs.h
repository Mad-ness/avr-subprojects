#ifndef __GHAIRDEFS_H__
#define __GHAIRDEFS_H__

#define AIR_ADDRESS_SIZE        6
#define AIR_PEER_SIZE           6
#define AIR_MAX_PACKET_SIZE     32
#define AIR_MAX_DATA_SIZE       29

// Samples of commands are presented
// but you can define its own
#define AIR_ADDR_NULL                   0x0


// The below regular commands except the AIR_CMD_OUT_RESP
// mustn't have the higher 7th bit bit set to 1. It's
// allocated to the mentioned command. Bits 6 and 5 are also
// used to tell the operation result.
// Commands values must not exceed 0x1F
#define AIR_CMD_UNDEF                   0X00
#define AIR_CMD_IN_PING                 0x01
//#define AIR_CMD_OUT_PONG                0x02
#define AIR_CMD_IN_RESET                0x03
#define AIR_CMD_IN_GET_EEPROM           0x04
//#define AIR_CMD_OUT_GET_EEPROM          0x05
#define AIR_CMD_IN_WRITE_EEPROM         0x06
//#define AIR_CMD_OUT_WRITE_EEPROM_OK     0x07
//#define AIR_CMD_OUT_WRITE_EEPROM_FAIL   0x08
#define AIR_CMD_IN_DATA                 0x09
//#define AIR_CMD_OUT_DATA                0x0A
#define AIR_CMD_IN_CMD1                 0x0B
//#define AIR_CMD_OUT_CMD1                0x0C
#define AIR_CMD_IN_CMD2                 0x0D
//#define AIR_CMD_OUT_CMD2                0x0E
// This command is used by the GHAir::sendResponse method
#define AIR_CMD_OUT_RESP                0x80 // 7th bit is set to 1
#define AIR_CMD_OUT_RESP_UNDEF          0x00
#define AIR_CMD_OUT_RESP_FAIL           0x40
#define AIR_CMD_OUT_RESP_OK             0x60

// Relays functions:
// the address field sets the relay id (address=0 means relay1, etc)
#define AIR_CMD_IN_RELAY_GET_STATE      0x50    // Get a relay's state (OFF or RUN)
//#define AIR_CMD_OUT_RELAY_GET_STATE     0x51    // response
#define AIR_CMD_IN_RELAY_SET_MODE       0x52    // Enable of disable (AUTO, MANUAL) control by schedule
//#define AIR_CMD_OUT_RELAY_SET_MODE      0x53    // response
#define AIR_CMD_IN_RELAY_GET_OPER_TIME  0x54    // Get the operational time
//#define AIR_CMD_OUT_RELAY_GET_OPER_TIME 0x55    // response
#define AIR_CMD_IN_RELAY_SET_OPER_TIME  0x56    // Set the operational time
//#define AIR_CMD_OUT_RELAY_SET_OPER_TIME 0x57    // response
#define AIR_CMD_IN_RELAY_GET_MODE       0x58    // Returns the mode (auto, manual) in which the relay is
//#define AIR_CMD_OUT_RELAY_GET_MODE      0x59    // response


// #define AirResponseOk(cmd)              ( cmd |= (1<<7) | (1<<6) | (1<<5) )
// #define AirResponseFail(cmd)            ( cmd |= (1<<7) | (1<<6); cmd &= ~(1<<5) )
// #define AirResponseUndef(cmd)           ( cmd |= (1<<7); cmd &= ~((1<<6)|(1<<5)) )

inline static bool AirResponseHasSuccess(const uint8_t cmd) {
    return true ? ( cmd & 0x60 ) : false;
}

inline static uint8_t AirResponseOnCmd(const uint8_t cmd) {
    return cmd & 0x1F; // clears three high bits and returns remains
}

static uint8_t AirResponseOk(const uint8_t cmd) {
    uint8_t out_cmd = cmd;
    out_cmd |= (1<<7) | (1<<6) | (1<<5);
    return out_cmd;
}

static uint8_t AirResponseFail(const uint8_t cmd) {
    uint8_t out_cmd = cmd;
    out_cmd |= (1<<7) | (1<<6);
    out_cmd &= ~(1<<5);
    return out_cmd;
}


struct AirPacket {
    uint8_t command;
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
