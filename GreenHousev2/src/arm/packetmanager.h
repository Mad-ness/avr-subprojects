#ifndef __PACKETMANAGER_H__
#define __PACKETMANAGER_H__

#include <string>
#include <ghair.h>

using namespace std;

class UserPacket {
    private:
        string client_id;
        AirPacket rf24packet;
        bool has_sent;
#if __ARM_ARCH == 7 			// 32-bit ARM systems
        unsigned int received = 0;
        unsigned int responded = 0;
#elif __ARM_ARCH == 8			// 64-bit ARM systems
        unsigned long received = 0;
        unsigned long responded = 0;
#endif
    public:
		UserPacket &operator=(const UserPacket &pkt);
		void markAsSentOut(void);
		AirPacket &radiopacket();
};


#endif // __PACKETMANAGER_H__

