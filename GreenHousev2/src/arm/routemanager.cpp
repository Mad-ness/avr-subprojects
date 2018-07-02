#include <iostream>
#include <unordered_map>
#include <routemanager.h>
#include <proxy-api.h>
#include <device-api.h>
#include <string.h>
//#include "surlparser.h"



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

/**
 *
 * Adds a callback (handler) into the list of callbacks
 * One will be executed when an approriate call from the user.
 */
//void RouteManager::addCallback(callback_t cb) {

//}

/**
 * It calls a low-level API function in case if all checks passed
 *
 */
//void RouteManager::callAPI(const RouteItemsInfo_t &apifunc, RouteManager::CallRC *whatsdone) {
//}

bool 
RouteManager::isAccepted(const string &uri) {
    return true;
}

bool 
RouteManager::isValidURI(const string &uri) {
    return isAccepted(uri);
}

void
RouteManager::addDeviceCallback( const char *path, URLParams_t params, CallbackDevice_t cb) {
    try {
        proxy_callbacks.at(path); 
    } catch (std::out_of_range) {
        device_callbacks[path] = { params, cb };
    }
}

void
RouteManager::addProxyCallback( const char *path, URLParams_t params, CallbackProxy_t cb) {
    try {
        device_callbacks.at(path);
    } catch (std::out_of_range) {
        proxy_callbacks[path] = { params, cb };
    }
}


