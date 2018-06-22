#include "packetmanager.h"
#include <iostream>
#include <ghairdefs.h>
#include <ghair.h>

int main() {

    AirPacket pkt;
    pkt.command = 0x1;
    pkt.address = 0x2;
    pkt.length = 0x3;

    UserPacket p1("userxxx", pkt), p2;
    //std::cout << p1.str() << std::endl;
    p2.markAsSentOut();
    //std::cout << p2.str() << std::endl;

    PacketManager mgr;
    mgr.addRequest(p1);
    mgr.addRequest(p2);
    mgr.print();
    return 0;
}

