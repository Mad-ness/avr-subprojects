#include <ghair.h>
#include <ghairdefs.h>
#include "device-api.h"



namespace deviceapi {

void ping( GHAir *air, const UserArgs_t &arg, string *output ) {
    air->sendPing(); 
}

void uptime( GHAir *air, const UserArgs_t &arg, string *output ) {
    air->sendUptime();
}

void reset( GHAir *air, const UserArgs_t &args, string *output ) {
    air->sendResetBoard();
}

void readEEPROM( GHAir *air, const UserArgs_t &args, string *output ) {
    uint8_t address = std::stoi( args.at("address"), nullptr );
    air->sendReadEEPROM(address);
}

void writeEEPROM( GHAir *air, const UserArgs_t &args, string *output ) {
    // air->sendWriteEEPROM(address, value);
}

/*** working with pins ***/

void setPinOutput( GHAir *air, const UserArgs_t &args, string *output ) {
    uint8_t pin = std::stoi(args.at("pid"), nullptr);
    air->sendSetPinAsOutput(pin);
}

void setPinInput( GHAir *air, const UserArgs_t &args, string *output ) {
    uint8_t pin = std::stoi(args.at("pid"), nullptr);
    air->sendSetPinAsInput(pin);
}

void getPinMode( GHAir *air, const UserArgs_t &args, string *output ) {
    // air->sendGetPinMode(pin);
}


void setPinHigh( GHAir *air, const UserArgs_t &args, string *output ) {
    // air->sendSetPinHigh(pin);
}

void setPinLow( GHAir *air, const UserArgs_t &args, string *output ) {
    // air->sendSetPinLow(pin);
}

void getPinValue( GHAir *air, const UserArgs_t &args, string *output ) {
    // air->sendGetPinValue(pin);
}

void setPWMValue( GHAir *air, const UserArgs_t &args, string *output ) {
    // air->sendSetPWMValue(pin, value);
}

void getPWMValue( GHAir *air, const UserArgs_t &args, string *output ) {
    // air->sendGetPWMValue(pin);
}

/*** working with pins (end) ***/

};

