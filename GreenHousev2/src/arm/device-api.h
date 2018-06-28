#ifndef __DEVICE_API_H__
#define __DEVICE_API_H__


class GHAir;

namespace deviceapi {
    
    bool ping(GHAir *air);
    bool uptime(GHAir *air);
    bool reset(GHAir *air);
    bool readEEPROM(GHAir *air, const uint8_t address);
    bool writeEEPROM(GHAir *air, const uint8_t address, const int8_t value);

}; // namespace deviceapi

#endif // __DEVICE_API_H__

