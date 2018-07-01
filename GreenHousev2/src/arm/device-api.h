#ifndef __DEVICE_API_H__
#define __DEVICE_API_H__


class GHAir;

namespace deviceapi {
    /**
     * All functions returns True if the request has been sent and accepted by the 
     * remote board
     */

    const struct {
/*
        uint8_t input   = INPUT;
        uint8_t output  = OUTPUT;
        //uint8_t pullup  = INPUT_PULLUP;
        uint8_t low     = LOW;
        uint8_t high    = HIGH;
*/
        uint8_t input   = 1;
        uint8_t output  = 0;
        //uint8_t pullup  = INPUT_PULLUP;
        uint8_t low     = 0;
        uint8_t high    = 1;
    } pin;
    
    bool ping(GHAir *air, UserArgs_t &args);
    bool uptime(GHAir *air);
    bool reset(GHAir *air);
    bool readEEPROM(GHAir *air, const uint8_t address);
    bool writeEEPROM(GHAir *air, const uint8_t address, const int8_t value);


    //*** ADC functions ***//
    // Power Wide Modulation, set value 0..1023
    bool setPWMValue(GHAir *air, const uint8_t pin, const uint8_t value);
    // Request PWM value from 0..1023
    bool getPWMValue(GHAir *air, const uint8_t pin);

    //*** GPIO functions ***/
    bool setPinInput(GHAir *air, UserArgs_t &args);
    bool setPinOutput(GHAir *air, const uint8_t pin);
    bool getPinMode(GHAir *air, const uint8_t pin);
    bool setPinHigh(GHAir *air, const uint8_t pin);
    bool getPinValue(GHAir *air, const uint8_t pin);

}; // namespace deviceapi

#endif // __DEVICE_API_H__

