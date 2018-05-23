#ifdef AIR_SENDER_DEBUG
#define DEBUG_AIR

#include <Arduino.h>
#include <ghair.h>

GHAir air(7, 8, "00001");

void setup(void) {
    Serial.begin(115200);
    air.setup();
}

void loop(void) {
}



#endif // AIR_SENDER_DEBUG
