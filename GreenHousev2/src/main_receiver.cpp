#ifdef AIR_RECEIVER_DEBUG
#define DEBUG_AIR

#include <Arduino.h>
#include <ghair.h>

static uint8_t cycles_cnt = 0;


GHAir air(7, 8, "1Node", "2Node");


void handleData(AirPacket *pkt) {
    int8_t cmd = pkt->command;
    int8_t address = pkt->address;
    int8_t length = pkt->length;

    char str[40];
    sprintf(str, "%03d. Command 0x%02x, Address 0x%02x, Datalen: %02d (bytes)\n", cycles_cnt++, cmd, address, length);
    Serial.print(str);
    air.packet().flush();
    if ( pkt->command == AIR_CMD_PING ) {
        Serial.print("Received a Ping\n");
        digitalWrite(13, HIGH);
        delay(150);
        digitalWrite(13, LOW);
    }
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
    if ( millis() % 5000 == 0 ) {
        if ( air.cmdPing() ) {
            Serial.println("Ping command is sent successfully.");
        } else {
            Serial.println("Ping command failed to send.");
        }
    }
}

#endif // AIR_RECEIVER_DEBUG
