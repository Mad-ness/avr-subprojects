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
		struct {
#if __ARM_ARCH == 7 			// 32-bit ARM systems
        	unsigned int request_received = 0;
 			unsigned int request_senttoboard = 0;
        	unsigned int board_responded = 0;
#elif __ARM_ARCH == 8			// 64-bit ARM systems
        	unsigned long request_received = 0;
        	unsigned long request_senttoboard = 0;
			unsigned long board_responded = 0;
#endif
        } time;
    public:
		UserPacket &operator=(const UserPacket &pkt);
		void markAsSentOut(void);
		AirPacket &radiopacket();
};


#endif // __PACKETMANAGER_H__

