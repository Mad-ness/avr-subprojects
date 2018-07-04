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
    void ping(GHAir *air, const UserArgs_t &args, string *output);
    void uptime(GHAir *air, const UserArgs_t &args, string *output);
    void reset(GHAir *air, const UserArgs_t &args, string *output);
    void readEEPROM(GHAir *air, const UserArgs_t &args, string *output);
    void writeEEPROM(GHAir *air, const UserArgs_t &args, string *output);


    //*** ADC functions ***//
    // Power Wide Modulation, set value 0..1023
    void setPWMValue(GHAir *air, const UserArgs_t &args, string *output);
    // Request PWM value from 0..1023
    void getPWMValue(GHAir *air, const UserArgs_t &args, string *output);

    //*** GPIO functions ***/
    void setPinAsInput(GHAir *air, const UserArgs_t &args, string *output);
    void setPinAsOutput(GHAir *air, const UserArgs_t &args, string *output);
    void getPinMode(GHAir *air, const UserArgs_t &args, string *output);
    void setPinHigh(GHAir *air, const UserArgs_t &args, string *output);
    void setPinLow(GHAir *air, const UserArgs_t &args, string *output);

    void getPinValue(GHAir *air, const UserArgs_t &args, string *output);

}; // namespace deviceapi

#endif // __DEVICE_API_H__

