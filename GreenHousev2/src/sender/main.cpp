#include <Arduino.h>
#include <EEPROM.h>
#include <printf.h>
#include <ghair.h>

GHAir air(7, 8, "1Node", "2Node");
long long last_flash_on = 0;

#ifdef DEBUG_AIR
#define printlog(msg) Serial.print(msg)
#define printlogln(msg) Serial.println(msg)
#else
#define printlog(msg) ;
#define printlogln(msg) ;
#endif


uint8_t memaddr = 0;
int8_t memval = 0;
int8_t buffer = 0;



void ping_pong_game(AirPacket *pkt) {
    int8_t cmd = pkt->command;
    const int8_t address = pkt->address;
    const int8_t length = pkt->length;

    char str[80];
    /*
    sprintf(str, "%03d. Command 0x%02x, Address 0x%02x, Datalen: %02d (bytes)\n", cycles_cnt++, cmd, address, length);
    printlog(str);
    */
    int ee_cell;
    switch ( cmd ) {
        case AIR_CMD_OUT_RESP:
            int8_t resp_cmd = pkt->command & 0x1F;
            switch ( resp_cmd ) {
                case AIR_CMD_IN_PING:
                    printlogln("Pong received, remote node alive");
                    break;
                case AIR_CMD_IN_GET_EEPROM:
                    memcpy(&ee_cell, pkt->data, pkt->length);
                    sprintf(str, "Received EEPROM data %d from %2x address\0", ee_cell, address);
                    printlogln(str);
                    break;
                case AIR_CMD_IN_WRITE_EEPROM:
                    if ( AirResponseHasSuccess(pkt->command) ) {
                        printlogln("EEPROM updating OK");
                    } else {
                        printlogln("EEPROM updating FAIL");
                    }
                    break;
                case AIR_CMD_IN_CMD1:
                    struct {
                        uint8_t hour;
                        uint8_t minute;
                        uint8_t second;
                    } t;
                    memcpy(&t, pkt->data, pkt->length);
                    sprintf(str, "Remote time: %02d:%02d:%02d\n", t.hour, t.minute, t.second);
                    printlog(str);
                    break;
            }
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
    Serial.println("  ====[ Started working (sender) ]====");
    air.rf24()->printDetails();
}

long long old_time = 0;
long cnt = 0;

bool is_sent = false;

void loop2() {
    air.loop();
    printlogln("Kicking off a ball...");
}

void script() {
    long long mls = millis();
    if ( mls % 3000 == 0 ) {
        printlogln("  >> requesting remote time ...");
        air.sendPacket(AIR_CMD_IN_CMD1, AIR_ADDR_NULL, 0x0, 0x0);
    }
    else if ( mls % 4000 == 0 ) {
        printlogln("  >> sending PING ...");
        air.sendPing();
    } else if ( mls % 7000 == 0 ) {
        memaddr++;
        memval += 5;
        printlogln("  >> sending WRITE_EEPROM ...");
        air.sendWriteEEPROM(memaddr, memval);
    } else if ( mls % 11000 == 0 ) {
        printlogln("  >> sending READ_EEPROM ...");
        air.sendReadEEPROM(memaddr);
    }

    if ( memval > 200 ) {
        air.sendResetBoard();
        memval = 0;
    }
}

void loop(void) {
    air.loop();
    script();
    return ;
}
