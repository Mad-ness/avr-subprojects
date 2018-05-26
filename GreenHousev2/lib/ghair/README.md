= Description of the GHAir protocol

== Packet format

Each packet is wrapped into the AirPacket structure as described in the ghairdefs.h.
```c++
struct AirPacket {
    int8_t command;
    int8_t address;
    int8_t length;
    byte data[AIR_MAX_DATA_SIZE];
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
```
A packet size shouldn't exceed the 32 bytes, it's the RF24L01+ limitation. If it
exceeds then it gets truncated and invalid data is being sent.

The **address** considered each of command individually. It may holds a cell in
the EEPROM memory either a variable that fits in the uint8_t data type. This
field is not treated by anyway until it is done.

The **length** tells the data array length ( sizeof( data ) ).

The **data** is a byte array max size of which is 29 bytes (32-sizeof(command) + sizeof(address) + sizeof(length)).
This is used for transmitting any kind of data by radio.

== The Command field
The **command** field tells what command is sent. Most of the common commands described in the
ghairdefs.h header file, they start as AIR_CMD_.

There are the rules that applies to the **command**:
- is a int8_t value
- if the higher (7th) bit is set to 0, this is an initial command, the command is
  sent by the initiator.
- if the higher (7th) bit is set to 1, this is a response command, the response to
  the initiator that has asked the receiver recently.
- there is a special command AIR_CMD_OUT_RESP == 0xF0 that is used when a response
  a required. In that case the following rules are applying:
  * if bits 5-6 are set to 1, than a response should be treated as the successful
    execution of a requested action (True response)
  * if bit 6 is set to 1 and bit 6 is zero then the response is treated as the
    failed execution (False response)
  * other values are treated as undefined result of executing of requested
    action
- regular commands values should be over 0x1F. Only 5 lower bits might be used
  by regular commands

```
    +---------- if set it means the packet is a response
    |   +------ if set this and bit 5 is zero, it's a negative response
    |   |   +-- if this and bit 6 are set, the response is positive
    |   |   |
    7   6   5   4   3   2   1   0
  +---+---+---+---+---+---+---+---+
  | 1 | 1 | 1 | 0 | 0 | 0 | 0 | 0 | the command register
  +---+---+---+---+---+---+---+---+
                ^   ^   ^   ^   ^
                the place for storing a regular command id,
                like initiating a dialog command.
                Do not forget to clear the 7th bit.
```
Examples:

```c++
// Assuming that we've got a packet
AirPacket &pkt = (AirPacket)income_packet;
pkt.command = AIR_CMD_IN_PING;  // Received a ping command

/* Responses that we can reply */
// the reply command to the previous response, saying that
// the requested operation is done with success
pkt.command |= (1 << 7) | (1 << 6) | (1 << 5); // 11100000
// the reply command saying that the requested operation
// has failed and negative response will come
pkt.command |= (1 << 7) | (1 << 6); // 11000000

// the two commands make the response with undefined result
pkt.command |= (1 << 7); pkt.command &= ~(1 << 6) | (1 << 5);

// You can also define helpful functions
inline static AirResponseOk(const int8_t cmd) {
    int8_t out_cmd = cmd;
    out_cmd |= (1<<7) | (1<<6) | (1<<5);
    return out_cmd;
}
inline static AirResponseFail(const int8_t cmd) {
    int8_t out_cmd = cmd;
    out_cmd |= (1<<7) | (1<<6);
    out_cmd &= ~(1<<5);
    return out_cmd;
}
AirResponseOk(pkt.command);     // response with a positive result
AirResponseFail(pkt.command);   // response with a negative result
```
Some commands do not require to have a result. So you can ignore setting the
bits 5 and 6 and use any of the responses.