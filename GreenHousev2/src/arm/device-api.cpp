#include <ghair.h>
#include <ghairdefs.h>
#include "device-api.h"

#define MSG_ACCEPTED()  { *output += "{\"result\":\"accepted\"}"; }
#define MSG_DECLINED()  { *output += "{\"result\":\"declined\"}"; }

namespace deviceapi {

bool ping( GHAir *air, const UserArgs_t &arg, string *output ) {
    if ( air->sendPing() ) {
        MSG_ACCEPTED();
    } else {
        MSG_DECLINED();
    }
}

bool uptime( GHAir *air, const UserArgs_t &arg, string *output ) {
    return air->sendUptime();
}

bool reset( GHAir *air, const UserArgs_t &args, string *output ) {
    return air->sendResetBoard();
}

bool readEEPROM( GHAir *air, const UserArgs_t &args, string *output ) {
    uint8_t address = std::stoi( args.at("address"), nullptr );
    return air->sendReadEEPROM(address);
}

bool writeEEPROM( GHAir *air, const UserArgs_t &args, string *output ) {
    // air->sendWriteEEPROM(address, value);
    return false;
}

/*** working with pins ***/

bool setPinAsOutput( GHAir *air, const UserArgs_t &args, string *output ) {
    uint8_t pin = std::stoi(args.at("pid"), nullptr);
    return air->sendSetPinAsOutput(pin);
}

bool setPinAsInput( GHAir *air, const UserArgs_t &args, string *output ) {
    uint8_t pin = std::stoi(args.at("pid"), nullptr);
    return air->sendSetPinAsInput(pin);
}

bool getPinMode( GHAir *air, const UserArgs_t &args, string *output ) {
    // air->sendGetPinMode(pin);
    return false;
}


bool setPinHigh( GHAir *air, const UserArgs_t &args, string *output ) {
    // air->sendSetPinHigh(pin);
    return false;
}

bool setPinLow( GHAir *air, const UserArgs_t &args, string *output ) {
    // air->sendSetPinLow(pin);
    return false;
}

bool getPinValue( GHAir *air, const UserArgs_t &args, string *output ) {
    // air->sendGetPinValue(pin);
    return false;
}

bool setPWMValue( GHAir *air, const UserArgs_t &args, string *output ) {
    // air->sendSetPWMValue(pin, value);
    return false;
}

bool getPWMValue( GHAir *air, const UserArgs_t &args, string *output ) {
    // air->sendGetPWMValue(pin);
    return false;
}

/*** working with pins (end) ***/

};

