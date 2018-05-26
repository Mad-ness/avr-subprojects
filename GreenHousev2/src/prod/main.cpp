#include <Arduino.h>
#include <manager.h>
#include <ghdisplay.h>
#include <ghair.h>
#include <ghairdefs.h>
#include <ghrelay.h>

Manager manager;

volatile void AirHandler(AirPacket *packet) {
    Manager &mgr = manager;
    AirPacket &pkt = *packet;
    int8_t buf8 = -1;
    switch ( pkt.command ) {
        case AIR_CMD_IN_RELAY_GET_STATE:
            if ( pkt.address == AIR_RELAY_ID_0 ) {
                buf8 = mgr.relay1().isOn() == LOW ? AIR_RELAY_STATE_OFF : AIR_RELAY_STATE_RUN;
            }
            mgr.air().sendResponse(pkt, true, sizeof(buf8), &buf8 );
            break;
        case AIR_CMD_IN_RELAY_GET_MODE:
            if ( pkt.address == AIR_RELAY_ID_0 ) {
                uint8_t result = mgr.relay1().getEEPROM(RELAY_ADDR_MODE);
                mgr.air().sendResponse(pkt, result, &result, sizeof(result));
            }
            break;
        case AIR_CMD_IN_RELAY_GET_OPER_TIME:
            break;
        case AIR_CMD_IN_RELAY_SET_OPER_TIME:
            break;
    }
}

void setup(void) {
#ifdef DEBUG_AIR
    Serial.begin(115200);
#endif //
    manager.air().setHandler(AirHandler);
    manager.setup();
    manager.display().showWelcomePage();
}
void loop(void) {
    manager.loop();
}
