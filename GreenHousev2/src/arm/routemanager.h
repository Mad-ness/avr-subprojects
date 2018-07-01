#pragma once
#ifndef __ROUTEMANAGER_H__
#define __ROUTEMANAGER_H__

#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <unordered_map>
// #include <ghairdefs.h>
#include <surlparser.h>

using namespace std;

class GHAir;

typedef KeyValueMap_t UserArgs_t;
typedef vector<string> URLParams_t;
typedef bool(*CallbackDevice_t)(GHAir *air, UserArgs_t &args);
typedef bool(*CallbackProxy_t)(UserArgs_t &args, UserArgs_t *output);


struct DeviceRouteItemInfo_t {
//    const char *path; // all paths must be in lowercase
    URLParams_t args;
    CallbackDevice_t cb;
};

struct ProxyRouteItemInfo_t {
//    const char *path; // all paths must be in lowercase
    URLParams_t args;
    CallbackProxy_t cb;
};

typedef unordered_map<string, ProxyRouteItemInfo_t> ProxyCallbacksList_t;
typedef unordered_map<string, DeviceRouteItemInfo_t> DeviceCallbacksList_t;

#define REGISTER_DEVICE_CALLBACK( path, params, cb ) \
        { RouteManager::addDeviceCallback( path, params, cb )}


class RouteManager {
    private:
        enum class CallRC { inqueue, response }; // whether a call is queued or is an immediate response
        enum class Receiver { proxy, device };
        SUrlParser parser;
        string errmsg;
        static DeviceCallbacksList_t device_callbacks;
        static ProxyCallbacksList_t proxy_callbacks;
        bool isAccepted(const string &uri);
        // void callAPI(const RouteItemsInfo_t &apifunc, RouteManager::CallRC *whatsdone);
    public:
        const string &emsg() { return errmsg; };
        bool isValidURI(const string &uri); 
        string path() { return parser.path(); };
        KeyValueMap_t params() { return parser.params(); }
        static void addDeviceCallback( const char *path, URLParams_t params, CallbackDevice_t cb);
        static void addProxyCallback( const char *path, URLParams_t params, CallbackProxy_t cb);
};

#endif // __ROUTEMANAGER_H__

