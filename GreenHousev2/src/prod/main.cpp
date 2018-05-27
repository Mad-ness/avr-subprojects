#include <Arduino.h>
#include <manager.h>
#include <ghdisplay.h>
#include <ghair.h>
#include <ghairdefs.h>
#include <ghrelay.h>
#include "customcmds.h"

Manager manager;

void AirHandler(AirPacket *pkt) {
    Manager &mgr = manager;
    GHAir &air = mgr.air();
    int8_t buf8 = -1;

    if ( pkt->isResponse() ) {
        switch ( pkt->getCommand() ) {
            case AIR_CMD_RELAY_GET_STATE: {
                    bool state = mgr.relay1().isOn();
                    air.sendResponse(*pkt, true, sizeof(state), &state);
                    break;
                }
            case AIR_CMD_RELAY_SET_STATE:
                break;
            case AIR_CMD_RELAY_GET_MODE:
                break;
            case AIR_CMD_RELAY_SET_MODE:
                break;
            case AIR_CMD_RELAY_GET_OPERTIME: {
                    struct {
                        uint8_t hour;
                        uint8_t minute;
                        uint16_t length;
                    } s_time;
                    mgr.relay1().getOperationalTime(&s_time.hour, &s_time.minute, &s_time.length);
                    memcpy(&pkt->data, &s_time, pkt->length);
                    air.sendResponse(*pkt, true, sizeof(s_time), &s_time);
                    break;
                }
            case AIR_CMD_RELAY_SET_OPERTIME:
                break;
        }
    }
/*
    switch ( pkt.command ) {
        case AIR_CMD_RELAY_GET_STATE:
            if ( pkt.address == AIR_RELAY_ID_0 ) {
                buf8 = mgr.relay1().isOn() == LOW ? AIR_RELAY_STATE_OFF : AIR_RELAY_STATE_RUN;
            }
            mgr.air().sendResponse(pkt, true, sizeof(buf8), &buf8 );
            break;
        case AIR_CMD_RELAY_GET_MODE:
            if ( pkt.address == AIR_RELAY_ID_0 ) {
                uint8_t result = mgr.relay1().getEEPROM(RELAY_ADDR_MODE);
                mgr.air().sendResponse(pkt, result, &result, sizeof(result));
            }
            break;
        case AIR_CMD_RELAY_GET_OPERTIME:
            break;
        case AIR_CMD_RELAY_SET_OPERTIME:
            break;
    }
*/
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
