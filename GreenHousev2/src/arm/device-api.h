#ifndef __DEVICE_API_H__
#define __DEVICE_API_H__

#include <routemanager.h>

class GHAir;

namespace deviceapi {
/*
    const struct {
        uint8_t input   = INPUT;
        uint8_t output  = OUTPUT;
        //uint8_t pullup  = INPUT_PULLUP;
        uint8_t low     = LOW;
        uint8_t high    = HIGH;
        
        uint8_t input   = 1;
        uint8_t output  = 0;
        //uint8_t pullup  = INPUT_PULLUP;
        uint8_t low     = 0;
        uint8_t high    = 1;
    } pin;
*/    
    bool ping(GHAir *air, const UserArgs_t &args, string *output);
    bool uptime(GHAir *air, const UserArgs_t &args, string *output);
    bool reset(GHAir *air, const UserArgs_t &args, string *output);
    bool readEEPROM(GHAir *air, const UserArgs_t &args, string *output);
    bool writeEEPROM(GHAir *air, const UserArgs_t &args, string *output);


    //*** ADC functions ***//
    // Power Wide Modulation, set value 0..1023
    bool setPWMValue(GHAir *air, const UserArgs_t &args, string *output);
    // Request PWM value from 0..1023
    bool getPWMValue(GHAir *air, const UserArgs_t &args, string *output);

    //*** GPIO functions ***/
    bool setPinAsInput(GHAir *air, const UserArgs_t &args, string *output);
    bool setPinAsOutput(GHAir *air, const UserArgs_t &args, string *output);
    bool getPinMode(GHAir *air, const UserArgs_t &args, string *output);
    bool setPinHigh(GHAir *air, const UserArgs_t &args, string *output);
    bool setPinLow(GHAir *air, const UserArgs_t &args, string *output);

    bool getPinValue(GHAir *air, const UserArgs_t &args, string *output);

    bool getLocalTime(GHAir *air, const UserArgs_t &args, string *output);
    bool setLocalTime(GHAir *air, const UserArgs_t &args, string *output);


}; // namespace deviceapi

#endif // __DEVICE_API_H__

