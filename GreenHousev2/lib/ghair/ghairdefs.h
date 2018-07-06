#ifndef __GHAIRDEFS_H__
#define __GHAIRDEFS_H__

#include <inttypes.h>
#include <string.h>

#ifndef ARDUINO
typedef unsigned char byte;

#endif // ARDUINO

#define AIR_ADDRESS_SIZE                6
#define AIR_PEER_SIZE                   6
#define AIR_MAX_PACKET_SIZE             32
#define AIR_MAX_DATA_SIZE               28

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
#define AIR_CMD_IN_RESET                0x02
#define AIR_CMD_IN_GET_EEPROM           0x03
#define AIR_CMD_IN_WRITE_EEPROM         0x04
#define AIR_CMD_IN_DATA                 0x05
#define AIR_CMD_IN_UPTIME               0x06

// These defines are used by the GHAir::sendResponse method
#define AIR_CMD_RESP                    0x80 // 7th bit is set to 1
#define AIR_CMD_RESP_GOOD               0x40 // 6th bit is set to 1

inline static bool isAirResponse(const uint8_t cmd) {
    return ( cmd & AIR_CMD_RESP ) == AIR_CMD_RESP;
}

inline static bool AirResponseHasSuccess(const uint8_t cmd) {
    return true ? ( cmd & 0x40 ) == 0x40 : false;
}

inline static uint8_t getAirResponseOnCmd(const uint8_t cmd) {
    return cmd & 0x3F; // clears two high bits and returns remains
}

inline static void setAirResponseOk(uint8_t *cmd) {
    *cmd |= (1 << 7) | (1<<6);
}

inline static void setAirResponseFail(uint8_t *cmd) {
    *cmd |= (1 << 7);
    *cmd &= ~(1 << 6);
}


struct AirPacket {
    uint8_t command;
    //AirCommand command;
    uint8_t address;
    uint8_t request_id;
    uint8_t length;      // length of the data
    byte data[AIR_MAX_DATA_SIZE];

    AirPacket() {
        command = AIR_CMD_UNDEF;
        address = AIR_ADDR_NULL;
        request_id = 0;
        length = 0;
    }

    AirPacket(uint8_t cmd, uint8_t addr, uint8_t req_id, uint8_t len, void *d) {
        command = cmd;
        address = addr;
        length = len;
        request_id = req_id;
        memcpy(&data, d, len);
    }

    AirPacket &operator=(const AirPacket &pkt) {
        command = pkt.command;
        address = pkt.address;
        length = pkt.length;
        request_id = pkt.request_id;
        memcpy(&data, &pkt.data, sizeof(pkt.length));
        return *this;
    }

    // size of a entire packet
    inline uint8_t getCommand() {
        return command & 0x1F;
    }

    inline bool isResponse() {
        return ( command & AIR_CMD_RESP ) == AIR_CMD_RESP;
    }
    inline bool isGoodResponse() {
        return ( command & AIR_CMD_RESP & AIR_CMD_RESP_GOOD ) == ( AIR_CMD_RESP & AIR_CMD_RESP_GOOD );
    }
    void markAsResponse(const bool with_good_answer) {
        command |= ( 1 << 7 );
        if ( with_good_answer )
            command |= ( 1 << 6 );
        else
            command &= ~( 1 << 6 );
    }
    uint8_t size() {
        return sizeof(command) + sizeof(address) + sizeof(request_id) + sizeof(length) + length;
    }
    uint8_t size_all() {
        return sizeof(this);
    }
    void flush() {
        command = AIR_CMD_UNDEF; address = AIR_ADDR_NULL; length = 0x0;
        memset(&data, 0, AIR_MAX_DATA_SIZE);
    }
};

// More custom commands that can be redefined in the handlers
// If you need to allocate a handler for a command just define
// one using one of unsed below custom commands like this:
// #define MYCOMMAND        AIR_CMD_IN_CUSTOM_01
// Make sure that the commands do not overlap.
// Command IDs 0x00 - 0x10 are reserved for built-in commands, they listed above.
#define AIR_CMD_IN_CUSTOM_01            0x11
#define AIR_CMD_IN_CUSTOM_02            0x12
#define AIR_CMD_IN_CUSTOM_03            0x13
#define AIR_CMD_IN_CUSTOM_04            0x14
#define AIR_CMD_IN_CUSTOM_05            0x15
#define AIR_CMD_IN_CUSTOM_06            0x16
#define AIR_CMD_IN_CUSTOM_07            0x17
#define AIR_CMD_IN_CUSTOM_08            0x18
#define AIR_CMD_IN_CUSTOM_09            0x19
#define AIR_CMD_IN_CUSTOM_10            0x1a
#define AIR_CMD_IN_CUSTOM_11            0x1b
#define AIR_CMD_IN_CUSTOM_12            0x1c
#define AIR_CMD_IN_CUSTOM_13            0x1d
#define AIR_CMD_IN_CUSTOM_14            0x1e
#define AIR_CMD_IN_CUSTOM_15            0x1f
#define AIR_CMD_IN_CUSTOM_16            0x20
#define AIR_CMD_IN_CUSTOM_17            0x21
#define AIR_CMD_IN_CUSTOM_18            0x22
#define AIR_CMD_IN_CUSTOM_19            0x23
#define AIR_CMD_IN_CUSTOM_20            0x24
#define AIR_CMD_IN_CUSTOM_21            0x25
#define AIR_CMD_IN_CUSTOM_22            0x26
#define AIR_CMD_IN_CUSTOM_23            0x27
#define AIR_CMD_IN_CUSTOM_24            0x28
#define AIR_CMD_IN_CUSTOM_25            0x29
#define AIR_CMD_IN_CUSTOM_26            0x2a
#define AIR_CMD_IN_CUSTOM_27            0x2b
#define AIR_CMD_IN_CUSTOM_28            0x2c
#define AIR_CMD_IN_CUSTOM_29            0x2d
#define AIR_CMD_IN_CUSTOM_30            0x2e
#define AIR_CMD_IN_CUSTOM_31            0x2f
#define AIR_CMD_IN_CUSTOM_32            0x30
#define AIR_CMD_IN_CUSTOM_33            0x31
#define AIR_CMD_IN_CUSTOM_34            0x32
#define AIR_CMD_IN_CUSTOM_35            0x33
#define AIR_CMD_IN_CUSTOM_36            0x34
#define AIR_CMD_IN_CUSTOM_37            0x35
#define AIR_CMD_IN_CUSTOM_38            0x36
#define AIR_CMD_IN_CUSTOM_39            0x37
#define AIR_CMD_IN_CUSTOM_40            0x38
#define AIR_CMD_IN_CUSTOM_41            0x39
#define AIR_CMD_IN_CUSTOM_42            0x3a
#define AIR_CMD_IN_CUSTOM_43            0x3b
#define AIR_CMD_IN_CUSTOM_44            0x3c
#define AIR_CMD_IN_CUSTOM_45            0x3d
#define AIR_CMD_IN_CUSTOM_46            0x3e
#define AIR_CMD_IN_CUSTOM_47            0x3f


#define AIR_CMD_SET_PIN_MODE            AIR_CMD_IN_CUSTOM_01
#define AIR_CMD_GET_PIN_MODE            AIR_CMD_IN_CUSTOM_02
#define AIR_CMD_SET_PIN_VALUE           AIR_CMD_IN_CUSTOM_03
#define AIR_CMD_GET_PIN_VALUE           AIR_CMD_IN_CUSTOM_04
#define AIR_CMD_SET_PWM_VALUE           AIR_CMD_IN_CUSTOM_05
#define AIR_CMD_GET_PWM_VALUE           AIR_CMD_IN_CUSTOM_06


#endif // __GHAIRDEFS_H__
