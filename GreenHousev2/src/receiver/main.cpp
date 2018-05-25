#include <Arduino.h>
#include <EEPROM.h>
#include <printf.h>
#include <ghair.h>

#ifdef DEBUG_AIR
#define printlog(msg) Serial.print(msg)
#define printlogln(msg) Serial.println(msg)
#define printflush() Serial.flush()
#else
#define printlog(msg)   ;
#define printlogln(msg) ;
#define printflush()    ;
#endif

uint8_t memaddr = 0;
int8_t memval = 0;
int8_t buffer = 0;

uint8_t time_hour = 0;
uint8_t time_minute = 0;
uint8_t time_second = 0;


GHAir air(7, 8, "2Node", "1Node");
long long last_flash_on = 0;

void ping_pong_game(AirPacket *pkt) {
    int8_t cmd = pkt->command;
    int8_t address = pkt->address;
    int8_t length = pkt->length;

    char str[80];
/*
    sprintf(str, "%03d. Command 0x%02x, Address 0x%02x, Datalen: %02d (bytes)\n", cycles_cnt++, cmd, address, length);
    printlog(str);
*/
    int ee_cell;
    switch ( pkt->command ) {
        case AIR_CMD_OUT_PONG:
            printlogln("Pong received, remote node  alive");
            break;
        case AIR_CMD_OUT_GET_EEPROM:
            memcpy(&ee_cell, pkt->data, pkt->length);
            sprintf(str, "Received EEPROM data %d from %2x address\0", ee_cell, address);
            printlogln(str);
            break;
        case AIR_CMD_OUT_WRITE_EEPROM_OK:
            printlogln("EEPROM updating OK");
            break;
        case AIR_CMD_OUT_WRITE_EEPROM_FAIL:
            printlogln("EEPROM updating FAIL");
            break;
        case AIR_CMD_IN_CMD1:
            struct {
                uint8_t hour = time_hour;
                uint8_t minute = time_minute;
                uint8_t second = time_second;
            } pkt_time;
            air.sendPacket(
                AIR_CMD_OUT_CMD1,
                AIR_ADDR_NULL,
                sizeof(pkt_time),
                &pkt_time
            );
            break;
    }
}

void setup(void) {
    Serial.begin(115200);
    printf_begin();
    air.setup();
    air.setHandler(&ping_pong_game);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    printlogln("  ====[ Started working (receiver) ]====");
    air.rf24()->printDetails();
}

long long old_time = 0;

void loop(void) {
    air.loop();
    if ( millis() % 1000 == 0 ) {
        if ( (++time_second) >= 60 ) {
            time_second = 0;
            if ( (++time_minute) >= 60 ) {
                time_minute = 0;
                time_hour++;
            }
        }
        char str[46];
        sprintf(str, "Current time: %02d:%02d:%02d\n",
                time_hour, time_minute, time_second);
        printlog(str);
    }
    return;
}
