#ifndef __PACKETMANAGER_H__
#define __PACKETMANAGER_H__

#include <string>
#include <list>
#include <iostream>
#include <ghair.h>

using namespace std;

#if __ARM_ARCH == 7
    typedef unsigned int packet_time_t;
#elif __ARM_ARCH == 8
    typedef unsigned long packet_time_t;
#endif

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
        uint32_t usedAttempts();
        packet_time_t &timeOfLastAttempt();
        packet_time_t &timeAddInQueue();
        uint32_t packetId();
        void setPacketId(const uint32_t &id);
        void setSentTime(packet_time_t t);
        packet_time_t getSentTime();
        string clientId();
        void setClientId(const string id);
};


class PacketManager {
    private:
        uint32_t curr_packet_id = 0;
        std::list<UserPacket> m_packets;
        void incrementPacketId(UserPacket &pkt);
    public:
        void addRequest(const UserPacket &packet);
        bool updateWithResponse(AirPacket &pkt);
        // next packet that is being sent
        UserPacket &nextPacket(bool *result);
        void print();
};

#endif // __PACKETMANAGER_H__

