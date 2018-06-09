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
    time_t received;         // a local time when the request come from a user
    time_t responded;        // indicates when the board received a reply from a remote board
    AirPacket packet;        // raw data received from a remote board
    HttpRequest_t() {};
    HttpRequest_t(const HttpRequest_t &src) {
            received = src.received;
            responded = src.responded;
            memcpy(&packet, &src.packet, sizeof(src.packet)); 
            strcpy(id, src.id);
        }
};

class DataCollector {
    private:
        std::list<HttpRequest_t> m_requests;
    public:
        ~DataCollector();
        void addRequest( const HttpRequest_t *request );
        void updateWithResponse(const char *id, AirPacket *pkt);
        void removeRequest( const char *id );
        void printContent();
};

#endif // __QUEUE_H__

