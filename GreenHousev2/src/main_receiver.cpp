#ifdef AIR_RECEIVER_DEBUG
#define DEBUG_AIR

#include <Arduino.h>
#include <ghair.h>

GHAir air(7, 8, "00001");
void setup(void) {
    Serial.begin(115200);
    air.setup();
}

static uint8_t cycles_cnt = 0;

void loop(void) {
    if ( millis() % 1000 == 0 ) {

        air.loop();
        if ( air.hasData() ) {

            AirPacket pkt = air.packet();
            int8_t cmd = pkt.command;
            int8_t address = pkt.address;
            int8_t length = pkt.length;

            char str[40];
            sprintf(str, "%03d. Command 0x%02x, Address 0x%02x, Datalen: %02d (bytes)\n", cycles_cnt++, cmd, address, length);
            Serial.print(str);
        }
    }
    if ( millis() % 5000 == 0 ) {
        air.cmdPing();
    }
}

#endif // AIR_RECEIVER_DEBUG
