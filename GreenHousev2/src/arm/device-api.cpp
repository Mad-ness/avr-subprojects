#include <ghair.h>
#include <ghairdefs.h>
#include "device-api.h"



namespace deviceapi {

bool ping(GHAir *air) {
    return air->sendPing(); 
}

bool uptime(GHAir *air) {
    return air->sendUptime();
}

bool reset(GHAir *air) {
    return air->sendResetBoard();
}

bool readEEPROM(GHAir *air, const uint8_t address) {
    return air->sendReadEEPROM(address);
}

bool writeEEPROM(GHAir *air, const uint8_t address, const int8_t value) {
    return air->sendWriteEEPROM(address, value);
}

};

