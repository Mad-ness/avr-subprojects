#include <stdio.h>
#include <ghair.h>

GHAir air(123, 120, (byte*)"1Node", (byte*)"2Node");

void AirHandler(AirPacket *pkt) {

}

int main() {

    air.setup();
    air.setHandler(&AirHandler);
    air.rf24()->printDetails();
    return 0;
}

