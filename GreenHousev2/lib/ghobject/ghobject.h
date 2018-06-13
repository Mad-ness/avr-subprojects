#ifndef __GHOBJECT_H__
#define __GHOBJECT_H__

#include <inttypes.h>

class GHObject {
private:
public:
    /*
    static GHAir air;
    static void setAirHandler(GHAir::on_packet_handler_t handler) {
        air.setHandler(handler);
    }
//    static GHAir &air() { return m_air; };
    */
    virtual void loop(void);
    virtual void setup(void);
};

class GHObjectEx: public GHObject {
private:
    int m_address;
public:
    /* It returns the address + sizeof(this) */
    void setEEPROMBaseAddr(int address);
    int getEEPROMBaseAddr();
    /* these two functions use relative address.
       Absolute address calculates as m_address + address.
       Where m_address is set by call of setEEPROMBaseAddr.
     */
    bool setEEPROM(int address, uint8_t value);
    uint8_t getEEPROM(int address);
};

#endif //__GHOBJECT_H__
