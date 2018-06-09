#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <string.h>
#include <time.h>
#include <ghairdefs.h>

#define REQUEST_ID_LEN 32
struct HttpRequest_t { 
    char id[REQUEST_ID_LEN]; // uniq strings that comes from withing a user's request
    time_t recieved;         // a local time when the request come from a user
    time_t responded;        // indicates when the board received a reply from a remote board
    AirPacket packet;        // raw data received from a remote board
};

class DataCollector {

    public:
        DataCollector();
        ~DataCollector();
        void addRequest( const HttpRequest_t &request ) {

        }
        void remoteRequest( const char *id ) {


        }

};

#endif // __QUEUE_H__

