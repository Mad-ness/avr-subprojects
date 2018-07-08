#include <Arduino.h>
#include <EEPROM.h>
#include <printf.h>
#include <ghair.h>
#include <ghairdefs.h>
#include <Time.h>
#include <time.h>

#ifdef DEBUG_AIR
#define printlog(msg) Serial.print(msg)
#define printlogln(msg) Serial.println(msg)
#define printflush() Serial.flush()
#else
#define printlog(msg)   ;
#define printlogln(msg) ;
#define printflush()    ;
#endif

#define AIR_CMD_REQ_TIME    AIR_CMD_IN_CUSTOM_01

uint8_t memaddr = 0;
int8_t memval = 0;
int8_t buffer = 0;

uint8_t time_hour = 0;
uint8_t time_minute = 0;
uint8_t time_second = 0;

time_t local_time;

GHAir air(7, 8, "2Node", "1Node");
long long last_flash_on = 0;

void ping_pong_game(AirPacket *pkt) {
    if ( pkt->isResponse() ) {
        switch ( pkt->getCommand() ) {
            case AIR_CMD_IN_PING:
                printlogln("[ooo] Remote host is alive");
                break;
            case AIR_CMD_IN_UPTIME:
                char str[40];
                unsigned long uptime;
                memcpy(&uptime, pkt->data, pkt->length);
                sprintf(str, "Remote board uptime %d seconds", uptime/1000);
                break;
            case AIR_CMD_IN_GETTIME: {
                memcpy(&local_time, &pkt->data, sizeof(local_time));
                char str[40];
                sprintf(str, "%d/%02d/%02d %02d:%02d:%02d\n", year(local_time), month(local_time), day(local_time), hour(local_time), minute(local_time), second(local_time));
                Serial.print(str);
            }; break;
        }
    } else {
        switch ( pkt->command ) {
            case AIR_CMD_REQ_TIME:
                struct {
                    uint8_t hour = time_hour;
                    uint8_t minute = time_minute;
                    uint8_t second = time_second;
                } pkt_time;
                air.sendResponse(*pkt, false, sizeof(pkt_time), &pkt_time);
                char str[30];
                sprintf(str, "  <<<< Sent time %02d:%02d:%02d\n", pkt_time.hour, pkt_time.minute, pkt_time.second);
                printlog(str);
                break;
        }
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
    time_t t = now();
    char time_str[40];
    sprintf(time_str, "%d/%02d/%02d %02d:%02d:%02d\n", year(t), month(t), day(t), hour(t), minute(t), second(t));
    Serial.print(time_str);
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
        if ( millis() % 10000 == 0 ) {
            char str[30];
            sprintf(str, "Current time: %02d:%02d:%02d\n",
                    time_hour, time_minute, time_second);
            printlog(str);
            air.sendUptime();
        } else if ( millis() % 11000 == 0 ) {
            air.sendPacket( AIR_CMD_IN_GETTIME, 0x0, 0, 0x0, NULL );
        } else if ( millis() % 7000 == 0 ) {
           // air.sendPing();
        } else if ( millis() % 4000 == 0 ) {
            char str[30];
            sprintf(str, "This board uptime is %u seconds\n", millis()/1000);
            printlog(str);
        }
    }
    return;
}
