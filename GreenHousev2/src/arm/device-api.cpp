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

/*** working with pins ***/

bool setPinOutput(GHAir *air, const uint8_t pin) {
    return air->sendGetPinOutput(pin);
}

bool setPinInput(GHAir *air, const uint8_t pin) {
    return air->sendSetPinInput(pin);
}

bool getPinMode(GHAir *air, const uint8_t pin) {
    return air->sendGetPinMode(pin);
}


bool setPinHigh(GHAir *air, const uint8_t pin) {
    return air->sendSetPinHigh(pin);
}

bool setPinLow(GHAir *air, const uint8_t pin) {
    return air->sendSetPinLow(pin);
}

bool getPinValue(GHAir *air, const uint8_t pin) {
    return air->sendGetPinValue(pin);
}

bool setPWMValue(GHAir *air, const uint8_t pin, const uint8_t value) {
    return air->sendSetPWMValue(pin, value);
}

bool getPWMValue(GHAir *air, const uint8_t pin) {
    return air->sendGetPWMValue(pin);
}

/*** working with pins (end) ***/

};

