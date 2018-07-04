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
    void reset(GHAir *air, UserArgs_t &args, string *output);
    void readEEPROM(GHAir *air, const UserArgs_t &args, string *output);
    void writeEEPROM(GHAir *air, const UserArgs_t &args, string *output);


    //*** ADC functions ***//
    // Power Wide Modulation, set value 0..1023
    void setPWMValue(GHAir *air, const uint8_t pin, const uint8_t value);
    // Request PWM value from 0..1023
    void getPWMValue(GHAir *air, const uint8_t pin);

    //*** GPIO functions ***/
    void setPinInput(GHAir *air, UserArgs_t &args);
    void setPinOutput(GHAir *air, const uint8_t pin);
    void getPinMode(GHAir *air, const uint8_t pin);
    void setPinHigh(GHAir *air, const uint8_t pin);
    void getPinValue(GHAir *air, const uint8_t pin);

}; // namespace deviceapi

#endif // __DEVICE_API_H__

