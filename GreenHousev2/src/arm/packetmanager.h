#ifndef __PACKETMANAGER_H__
#define __PACKETMANAGER_H__

#include <string>
#include <list>
#include <iostream>
//#include <nlohmann/json.hpp>
#include <ghair.h>

using namespace std;
//using json = nlohmann::json;

#if __ARM_ARCH == 7
    typedef unsigned int packet_time_t;
#elif __ARM_ARCH == 8
    typedef unsigned long packet_time_t;
#endif


const int UPTimeReceived          = 0b00000001;
const int UPTimeSentToBoard       = 0b00000010;
const int UPTimeBoardResponded    = 0b00000100;
const int UPClientId              = 0b00001000;
const int UPHasSentFlag           = 0b00010000;
const int UPUsedAttempts          = 0b00100000;
const int UPPacketId              = 0b01000000;
const int UPAirPacket             = 0b10000000;

class UserPacket {
    private:
        string client_id;
        uint32_t packet_id;
        // number of previous attempts to send the packet
        uint32_t used_attempts;
        AirPacket rf24packet;
        bool has_sent = false;
        struct {
            packet_time_t request_received;
            packet_time_t request_senttoboard;
            packet_time_t board_responded;
        } when;
        void init();
    public:
        UserPacket();
        UserPacket(const string c_id, const AirPacket &pkt);
        bool operator==(const UserPacket &pkt);

        void attemptedToSend(bool with_success);
        bool hasShipped();
        AirPacket &airpacket();
        void updateRadioPacket(AirPacket &pkt);
        string str(void);
        void replacePacket(const AirPacket &pkt);
        void replacePacket(const UserPacket &pkt, const int flags);
        uint32_t usedAttempts();
        packet_time_t &timeOfLastAttempt();
        packet_time_t &timeAddInQueue();
        uint32_t packetId();
        void setPacketId(const uint32_t &id);
        string clientId();
        void setClientId(const string id);
        const packet_time_t &getSentTime();
};


class PacketManager {
    private:
        uint32_t curr_packet_id = 0;
        std::list<UserPacket> m_packets;
        void incrementPacketId(UserPacket &pkt);
    public:
        UserPacket &addRequest(const UserPacket &packet);
        bool updateWithResponse(AirPacket &pkt);
        // next packet that is being sent
        UserPacket &nextPacket(bool *result);
        void print();
};

#endif // __PACKETMANAGER_H__

