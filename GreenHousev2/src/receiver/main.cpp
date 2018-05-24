#include <Arduino.h>
#include <printf.h>
#include <ghair.h>

uint8_t cycles_cnt = 0;

#ifdef DEBUG_AIR
#define printlog(msg) Serial.print(msg)
#define printlogln(msg) Serial.println(msg)
#else
#define printlog(msg) ;
#define printlogln(msg) ;
#endif

GHAir air(7, 8, "2Node", "1Node");
long long last_flash_on = 0;


void handleData(AirPacket *pkt) {
    int8_t cmd = pkt->command;
    int8_t address = pkt->address;
    int8_t length = pkt->length;

    char str[80];
    sprintf(str, "%03d. Command 0x%02x, Address 0x%02x, Datalen: %02d (bytes)\n", cycles_cnt++, cmd, address, length);
    printlog(str);
//    air.packet().flush();
    char msg[50];
    switch ( cmd ) {
        case AIR_CMD_UNDEF:
            sprintf(msg, "     Undefined command received. Do nothing");
            break;
        case AIR_CMD_PING:
            if ( air.cmdPong() ) {
                sprintf(msg, "[ OK ] Ping received and Pong is sent");
            } else {
                sprintf(msg, "[FAIL] Ping received and Pong failed to send");
            }
            break;
        case AIR_CMD_PONG:
            if ( air.cmdPing() ) {
                sprintf(msg, "[ OK ] Pong received and Ping is sent");
            } else {
                sprintf(msg, "[FAIL] Pong received and Pong failed to send");
            }
            break;
    }
    Serial.println(msg);
    digitalWrite(LED_BUILTIN, HIGH);
    last_flash_on = millis();
}

void ping_pong_game(AirPacket *pkt) {
    int8_t cmd = pkt->command;
    int8_t address = pkt->address;
    int8_t length = pkt->length;

    char str[80];
    sprintf(str, "%03d. Command 0x%02x, Address 0x%02x, Datalen: %02d (bytes)\n", cycles_cnt++, cmd, address, length;
    printlog(str);

    switch ( pkt->command ) {
        case AIR_CMD_DATA:
            char str[30];
            uint16_t data;
            memcpy(&data, pkt->data, pkt->length);
            sprintf(str, "Received value: %03d", data++);
            printlogln(str);
            delay(1000);
            while ( ! air.cmdSendData(&data, pkt->length) );
            break;
    }}

void setup(void) {
    Serial.begin(115200);
    printf_begin();
    air.setup();
    air.onGetData(&ping_pong_game);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    printlogln("  ====[ Started working (receiver) ]====");
    air.rf24()->printDetails();
}

long long old_time = 0;

void loop(void) {
    air.loop();

    return;
    if ( digitalRead(LED_BUILTIN) == HIGH && millis() - last_flash_on > 500 ) {
        digitalWrite(LED_BUILTIN, LOW);
    }
    if ( millis() - old_time > 1100 ) {
        if ( air.cmdPing() ) {
            printlogln("Regular Ping command is sent.");
        } else {
            printlogln("Failed to sent a regular Ping packet");
        }
        old_time = millis();
    }
}
