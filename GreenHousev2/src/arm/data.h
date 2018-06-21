#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <list>
#include <string.h>
#include <time.h>
#include <ghairdefs.h>
#ifdef DEBUG
#include <stdio.h>
#endif


#define REQUEST_ID_LEN 32
class HttpRequest_t { 
    public: 
    char id[REQUEST_ID_LEN]; // uniq strings that comes from withing a user's request
    bool hasSent = false;
#if __ARM_ARCH == 7
    unsigned int received = 0;
    unsigned int responded = 0;
#elif __ARM_ARCH == 8
    unsigned long received = 0;
    unsigned long responded = 0;
#endif 
    //time_t received;         // a local time when the request come from a user
    //time_t responded;        // indicates when the board received a reply from a remote board
    AirPacket packet;        // raw data received from a remote board
    HttpRequest_t() {};
    void markAsSentOut() {
        received = time(NULL); 
        hasSent = true;
    }
    HttpRequest_t(const HttpRequest_t &src) {
            hasSent = src.hasSent;
            received = src.received;
            responded = src.responded;
            packet = src.packet;
            //memcpy(&packet, &src.packet, sizeof(src.packet)); 
            strcpy(id, src.id);
        }
    bool operator==(const HttpRequest_t &src) {
        return (
            ( this->packet.command == src.packet.command ) &&
            ( this->packet.address == src.packet.address ) &&
            ( this->packet.length == src.packet.length )
        );
    }
};

class DataCollector {
    private:
        std::list<HttpRequest_t> m_requests;
    public:
        std::list<HttpRequest_t> &data() { return this->m_requests; };
        ~DataCollector();
        void addRequest( const HttpRequest_t &request );
        void updateWithResponse(const char *id, AirPacket *pkt);
        void removeRequest( const char *id );
        void printContent();
        HttpRequest_t *byCommand(const uint8_t cmd);
        void updateWithResponse(AirPacket &pkt);
        HttpRequest_t *packetToSend(void);
};

#endif // __QUEUE_H__

