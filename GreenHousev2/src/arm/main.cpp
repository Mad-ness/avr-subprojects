#include <stdio.h>
#include <unistd.h>
#include <ghair.h>

#include <lev.h>
#include <levhttp.h>

using namespace lev;

GHAir air(42, 266, (byte*)"1Node", (byte*)"2Node");

void AirHandler(AirPacket *pkt) {

}

int main() {

    air.setup();


    while ( ! air.rf24()->isChipConnected() ) {
        printf("Chip is not connected. Checking again in 5 seconds ");
        int i = 0;
        while ( i++ < 5 ) {
            sleep(1);
            printf(".");
        }
        sleep(5);
        printf("\n");
    }
    printf("Chip is connected.\n");

    air.setHandler(&AirHandler);
    air.rf24()->setPALevel(RF24_PA_HIGH);
    air.rf24()->printDetails();

    while ( 1 ) {
        printf("Trying to send a ping...\n");
        sleep(5);
        if ( air.sendPing() ) {
            printf("Ping is sent out.\n");
        } else { 
            printf("Ping did not send.\n");
        }
    }

    return 0;
}

