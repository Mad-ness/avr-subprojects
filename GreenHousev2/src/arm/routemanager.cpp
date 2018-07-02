#include <iostream>
#include <unordered_map>
#include <routemanager.h>
#include <proxy-api.h>
#include <device-api.h>
#include <string.h>
//#include "surlparser.h"


static DeviceCallbacksList_t device_callbacks;
static ProxyCallbacksList_t proxy_callbacks;

//typedef unordered_map<string, ProxyRouteItemInfo_t> ProxyCallbacksList_t;
//typedef unordered_map<string, DeviceRouteItemInfo_t> DeviceCallbacksList_t;



//#define REGISTER_PROXY_CALLBACK( path, params, callback ) \
//        ( ProxyCallbacksList.push_back( { ProxyRouteItemInfo_t(path, URLParams_t(params), callback })))

//#define REGISTER_DEVICE_CALLBACK( path, params, callback ) \
//        ( DeviceCallbacksList.push_back( { path, URLParams_t(params), callback }))


/*
RouteItemsInfo_t RouteItemsInfo[] = {                                            // this array mustn't be empty{
    { "dummy",                              URLParams_t(), proxyapi::dummy },    // keep this record always here
    { "/device/ping",                       URLParams_t(), deviceapi::ping },
    { "/device/pin/value/set",              URLParams_t({ "did", "pid" }), deviceapi::setPinInput },
    { "/proxy/ping",                        URLParams_t() },
    { "/proxy/uptime",                      URLParams_t() },
    { "/device/reset",                      URLParams_t({ "did" } )},
    { "/device/uptime",                     URLParams_t({ "did" } )},
    { "/device/getee",                      URLParams_t({ "did", "addr" } )},
    { "/device/setee",                      URLParams_t({ "did", "addr", "value" } )},
    { "/device/pin/set-in",                 URLParams_t({ "did", "pid" } )},
    { "/device/pin/set-out",                URLParams_t({ "did", "pid" } )},
    { "/device/pin/value",                  URLParams_t({ "did", "pid" } )},
    { "/device/pin/value/set0",             URLParams_t({ "did", "pid" } )},
    { "/device/pin/value/set1",             URLParams_t({ "did", "pid" } )},
};
*/


bool areArgsOk( const URLParams_t &mandatory_args, const KeyValueMap_t &passed_args) {
    for ( auto &arg : mandatory_args ) {
        if ( passed_args.find(arg) == passed_args.end() )
            return false;   // seek for the end and didn't find the entry
    }
    return true;
}


void
RouteManager::accept(const char *uri, string *outmsg) {
    // Check if it a request to the proxy
    if ( strncmp( "/proxy", uri, 6 ) == 0 ) {
        std::cout << "Proxy handler requested\n";
        try {
            ProxyRouteItemInfo_t *item = NULL;
            item = &proxy_callbacks.at(uri);
            parser.parse(uri);
            if ( areArgsOk( item->args, params() )) {
                item->cb(params(), outmsg);
            } else {
                *outmsg += "{\"accepted\":false,\"msg\":\"not all mandatory arguments passed\"}";
            }
        } catch ( std::out_of_range ) {
            *outmsg += "{\"accepted\":false,\"msg\":\"no such handler registered\"}";
        }
    } else if ( strncmp( "/device", uri, 7 ) == 0 ) {
        std::cout << "Device handler requested\n";
        try {
            DeviceRouteItemInfo_t *item = &device_callbacks.at(uri); 
        } catch ( std::out_of_range ) {
            *outmsg += "{\"accepted\":false,\"msg\":\"no such handler registered\"}";
        }
    } else {
        std::cout << "Not defined handler requested\n";
        *outmsg += "{\"accepted\":false,\"msg\":\"not supported handler type\"}";
    }
}

bool 
RouteManager::isAccepted(const string &uri) {
    return true;
}

bool 
RouteManager::isValidURI(const string &uri) {
    return isAccepted(uri);
}

void
addDeviceCallback( const char *path, URLParams_t params, CallbackDevice_t cb) {
    try {
        proxy_callbacks.at(path); 
    } catch (std::out_of_range) {
        device_callbacks[path] = { params, cb };
    }
}

void 
printHandlers() {
    for ( auto &handler : proxy_callbacks ) {
        std::cout << handler.first << std::endl;
    }
    for ( auto &handler : device_callbacks ) {
        std::cout << handler.first << std::endl;
    }
}

void
addProxyCallback( const char *path, URLParams_t params, CallbackProxy_t cb) {
    try {
        device_callbacks.at(path);
    } catch (std::out_of_range) {
        proxy_callbacks[path] = { params, cb };
    }
}


