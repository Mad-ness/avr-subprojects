#ifdef AIR_SENDER_DEBUG
#define DEBUG_AIR

#include <Arduino.h>
#include <ghair.h>

uint8_t cycles_cnt = 0;

GHAir air(7, 8, "2Node", "1Node");

void handleData(AirPacket *pkt) {
    int8_t cmd = pkt->command;
    int8_t address = pkt->address;
    int8_t length = pkt->length;

    char str[40];
    sprintf(str, "%03d. Command 0x%02x, Address 0x%02x, Datalen: %02d (bytes)\n", cycles_cnt++, cmd, address, length);
    Serial.print(str);
    air.packet().flush();
    char msg[50];
    switch ( cmd ) {
        case AIR_CMD_UNDEF:
            sprintf(msg, "Undefined command received. Do nothing");
            break;
        case AIR_CMD_PING:
            if ( air.cmdPong() ) {
                sprintf(msg, "Ping received and Pong is sent");
            } else {
                sprintf(msg, "Ping received and Pong failed to send");
            }
            break;
        case AIR_CMD_PONG:
            if ( air.cmdPing() ) {
                sprintf(msg, "Pong received and Ping is sent");
            } else {
                sprintf(msg, "Pong received and Pong failed to send");
            }
            break;
    }
    Serial.println(msg);
    digitalWrite(13, HIGH);
    delay(200);
    digitalWrite(13, LOW);
}

void setup(void) {
    Serial.begin(115200);
    air.setup();
    air.onGetData(&handleData);
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
}


void loop(void) {
    air.loop();
    delay(3000);
    air.cmdPing();
}

#endif // AIR_SENDER_DEBUG
