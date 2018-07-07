#pragma once
#ifndef __ROUTEMANAGER_H__
#define __ROUTEMANAGER_H__

#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <unordered_map>
#include <surlparser.h>
#include <apptypes.h>


using namespace std;

class GHAir;


/**
 * Adds an URL handler.
 * path - an URI (after the hostname:port and before the question "?" sign, like as /path1/path2/.../)
 * params - a list of mandatory parameters this handler accepts
 * cb - a callback which handles the URL
 */
void install_callbacks();

struct DeviceRouteItemInfo_t {
    URLParams_t args;
    URLParams_t ret_args;
    CallbackDevice_t cb;
};

struct ProxyRouteItemInfo_t {
    URLParams_t args;
    URLParams_t ret_args;
    CallbackProxy_t cb;
};

typedef unordered_map<string, ProxyRouteItemInfo_t> ProxyCallbacksList_t;
typedef unordered_map<string, DeviceRouteItemInfo_t> DeviceCallbacksList_t;

void addDeviceCallback( const char *path, const URLParams_t &params, const URLParams_t &ret_params, CallbackDevice_t cb);
void addProxyCallback( const char *path, const URLParams_t &params, const URLParams_t &ret_params, CallbackProxy_t cb);
void printHandlers();


#if __ARM_ARCH == 7
    typedef unsigned int request_time_t;
#elif __ARM_ARCH == 8
    typedef unsigned long request_time_t;
#else                                       // for everything else take maximum possible (64-bit) size
    typedef unsigned long request_time_t;
#endif

/**
 * Status of a request
 */
enum class RequestStatus { New, Sent, Completed };
struct RequestItem_t {
    string path;
    UserArgs_t args;
    string uri_hash;                // hashsum of uri
    int num_requests = 0;           // how many the same requests received
    int done_attempts = 0;          // how many attempts have been perfromed to transmit the request
    int failed_attempts = 0;        // how many attempts to send the request to the remote board failed
    // bool has_sent = false;          // indicates whether the request sent to the remote board
    RequestStatus status;           // indicates the status of the request
    string errmsg;                  // this contains the message if an error occured during sending
    int packet_id = 0;              // packet-id is sent to the remote board
    struct {
        request_time_t received;    // received from the end user
        request_time_t scheduled;   // transmitted to the remote board
        request_time_t completed;   // received a response from the remote board
    } when;
    // the request has been sent and waiting for a response from the remote board
    bool hasSent() {
        return ( status == RequestStatus::Sent );
    }
};



string
makeSHA256(const char *uri);

class RouteManager {
    private:
        // enum class CallRC { inqueue, response }; // whether a call is queued or is an immediate response
        enum class Receiver { undef, proxy, device };
        unordered_map<string, RequestItem_t> m_requests;
        SUrlParser parser;
        string errmsg;
        bool isAccepted(const string &uri);
        void callHandler(const char *uri, const Receiver rcv, string *outmsg);
        RequestItem_t *addRequestInQueue(const char *uri);
        void processRequestsQueue();
        string processResponse();
    public:
        GHAir *air;
        void accept(const char *uri, string *out_msg);
        const string &emsg() { return errmsg; };
        bool isValidURI(const string &uri); 
        string path() { return parser.path(); };
        KeyValueMap_t params() { return parser.params(); }
        void printInQueue();
        // processing the queue of queries, run this in a loop as often as possible
        void loop();
};

#endif // __ROUTEMANAGER_H__

