#include <Arduino.h>
#include <ghair.h>

uint8_t cycles_cnt = 0;

GHAir air(7, 8, "1Node", "2Node");
long long last_flash_on = 0;


void handleData(AirPacket *pkt) {
    int8_t cmd = pkt->command;
    int8_t address = pkt->address;
    int8_t length = pkt->length;

    char str[40];
    sprintf(str, "%03d. Command 0x%02x, Address 0x%02x, Datalen: %02d (bytes)\n", cycles_cnt++, cmd, address, length);
    Serial.print(str);
//    air.packet().flush();
    char msg[50];
    switch ( cmd ) {
        case AIR_CMD_UNDEF:
            sprintf(msg, "     Undefined command received. Do nothing");
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
    last_flash_on = millis();
}

void setup(void) {
    Serial.begin(57600);
    air.setup();
    air.onGetData(&handleData);
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
    Serial.println("  ====[ Started working (sender) ]====");
}

long long old_time = 0;
long cnt = 0;

void loop(void) {
    air.loop();
    if ( digitalRead(13) == HIGH && millis() - last_flash_on > 500 ) {
        digitalWrite(13, LOW);
    }
    if ( millis() - old_time > 1000 ) {
        Serial.println("Time to send a ping packet");
        if ( air.cmdPing() ) {
            Serial.println("Regular Ping command is sent.");
        } else {
            Serial.println("Failed to sent a regular Ping packet");
        }
        old_time = millis();
    }
//    if ( ++cnt % 100 == 0 )
//        Serial.println(cnt);
}
