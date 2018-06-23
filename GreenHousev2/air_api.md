# AirProto API

There is the documentation about the API that a remote board accepts and format of passed data.

**Format of the packet sent to the remote board**

```
#define     AIR_MAX_DATA_SIZE    29
typedef     unsigned char        byte;

struct AirPacket {
    uint8_t command;
    uint8_t address;
    uint8_t length;      
    byte data[AIR_MAX_DATA_SIZE];
```
Maximum possible packet size is 32 bytes. That is limited by nRF24L01 modules.

When a command sent to the remote board, it modifies the **command** byte by setting the highest (7) byte. It does not change the **address** byte. The **length** value and **data** content depends on the called function itself. The **length** byte tells how many bytes are stored in the **data** array. Format of data passed through the **data** array is depend on the called function.

## Functions

If there said that ***no changes*** in a response (output) it means that the **command** byte still modified, if not mentioned other.

**Shortages**
- ***N/U*** - not used
- ***N/A*** - not applicable
- ***REQ*** - required

#### AIR_CMD_UNDEF = 0x0
- description: undefined command, ignored by the remote board
- input data: addr: N/U len: N/U, data: N/Nu
- response: N/A
- other: no response should come

#### AIR_CMD_IN_PING = 0x1
- description: ping a remote board, if it responds - the board is alive
- input data: addr: N/U, len: N/U, data: N/U
- output: no changes

#### AIR_CMD_IN_RESET = 0x2
- description: make a soft reset using watchdog, completes immediately
- input data: addr: N/U, len: N/U, data: N/U
- output: no changes

#### AIR_CMD_IN_GET_EEPROM = 0x3
- description: undefined command, ignored
- input data: addr: REQ, len: N/U, data: N/U
- output: len: 1, data[0]: value read from the specified byte of EEPROM;

#### AIR_CMD_IN_WRITE_EEPROM = 0x4
- description: writes a byte into the EERPOM
- input data: addr: REQ, len: N/U, data[0]: value to be written ... to be clarified ...
- output: ... to be clarified ...

#### AIR_CMD_IN_DATA = 0x5
- description: returns a value from the specified byte of memory
- input data: addr: REQ, len: N/U, data[0]: value to be written ... to be clarified ...
- output: ... to be clarified ...

#### AIR_CMD_IN_UPTIME = 0x6
- description: returns uptime in microseconds (1/1000 second)
- input data: addr: N/U, len: N/U, data: N/U
- output: long long (8 bytes)

