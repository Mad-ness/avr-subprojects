#include "packetmanager.h"

UserPacket &UserPacket::operator=(const UserPacket &pkt) {
    client_id = pkt.client_id;
    has_sent = pkt.has_sent;
    rf24packet = pkt.rf24packet;
}

void UserPacket::markAsSentOut(void) {
    has_sent = true;
}

AirPacket &UserPacket::radiopacket() {
    return rf24packet;
}

