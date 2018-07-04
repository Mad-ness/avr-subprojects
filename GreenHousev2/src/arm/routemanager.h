#pragma once
#ifndef __ROUTEMANAGER_H__
#define __ROUTEMANAGER_H__

#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <unordered_map>
#include <surlparser.h>


using namespace std;

class GHAir;

typedef KeyValueMap_t UserArgs_t;
typedef vector<string> URLParams_t;
typedef void(*CallbackDevice_t)(GHAir *air, const UserArgs_t &args, string *output);
typedef void(*CallbackProxy_t)(const UserArgs_t &args, string *output);

/**
 * Adds an URL handler.
 * path - an URI (after the hostname:port and before the question "?" sign, like as /path1/path2/.../)
 * params - a list of mandatory parameters this handler accepts
 * cb - a callback which handles the URL
 */
void install_callbacks();

struct DeviceRouteItemInfo_t {
    URLParams_t args;
    CallbackDevice_t cb;
};

struct ProxyRouteItemInfo_t {
    URLParams_t args;
    CallbackProxy_t cb;
};

typedef unordered_map<string, ProxyRouteItemInfo_t> ProxyCallbacksList_t;
typedef unordered_map<string, DeviceRouteItemInfo_t> DeviceCallbacksList_t;

void addDeviceCallback( const char *path, const URLParams_t &params, CallbackDevice_t cb);
void addProxyCallback( const char *path, const URLParams_t &params, CallbackProxy_t cb);
void printHandlers();

class RouteManager {
    private:
        enum class CallRC { inqueue, response }; // whether a call is queued or is an immediate response
        enum class Receiver { proxy, device };
        SUrlParser parser;
        string errmsg;
        bool isAccepted(const string &uri);
    public:
        void accept(const char *uri, string *out_msg);
        const string &emsg() { return errmsg; };
        bool isValidURI(const string &uri); 
        string path() { return parser.path(); };
        KeyValueMap_t params() { return parser.params(); }
};

#endif // __ROUTEMANAGER_H__

