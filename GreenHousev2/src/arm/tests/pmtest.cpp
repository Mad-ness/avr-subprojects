#include "packetmanager.h"
#include <iostream>
#include <ghairdefs.h>
#include <ghair.h>

int main() {

    PacketManager mgr;
    mgr.addRequest(UserPacket("pack1", AirPacket(AIR_CMD_IN_PING, 0x0, 0x0, NULL)));
    mgr.addRequest(UserPacket("pack2", AirPacket(AIR_CMD_IN_UPTIME, 0x0, 0x0, NULL)));
    mgr.addRequest(UserPacket("pack3", AirPacket(AIR_CMD_IN_PING, 0x0, 0x0, NULL)));
    mgr.print();
    bool is_ok = false;
    UserPacket upkt = mgr.nextPacket(&is_ok);
    if ( ! is_ok ) {
        std::cout << "Not found a packet for sending" << std::endl;
    } else {
        std::cout << std::endl << "Packet for sending: " << upkt.str()
                  << std::endl << "Packet ID is zero: " << string((upkt.packetId() > 10) ? "yes" : "no") << std::endl;
    }
    return 0;
}

