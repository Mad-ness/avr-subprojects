#include <iostream>
#include <unordered_map>
#include <routemanager.h>
#include <proxy-api.h>
#include <device-api.h>
#include <string.h>


static DeviceCallbacksList_t device_callbacks;
static ProxyCallbacksList_t proxy_callbacks;


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


bool 
areArgsOk( const URLParams_t &mandatory_args, const KeyValueMap_t &passed_args) {
    for ( auto &arg : mandatory_args ) {
        if ( passed_args.find(arg) == passed_args.end() )
            return false;   // seek for the end and didn't find the entry
    }
    return true;
}


void
RouteManager::accept(const char *uri, string *outmsg) {
    if ( parser.parse(uri) ) {
        if ( strncmp( "/device", uri, 7 ) == 0 ) {
            callHandler( uri, Receiver::device, outmsg );
        } else if ( strncmp( "/proxy", uri, 6 ) == 0 ) {
            callHandler( uri, Receiver::proxy, outmsg );
        } else {
            *outmsg += "{\"accepted\":false,\"msg\":\"Not supported handler type\"}";
        }
    } else {
        *outmsg += "{\"accepted\":false,\"msg\":\"Cannot parse the url\"}";
    }
}

void
RouteManager::callHandler(const char *uri, const Receiver rcv, string *outmsg) {
    const string requested_handler_by_uri = parser.path();
    if ( rcv == Receiver::device ) {
        try {
            DeviceRouteItemInfo_t *item = &device_callbacks.at(requested_handler_by_uri);
            if ( areArgsOk( item->args, params() )) {
                // item->cb( &air, parser.params(), outmsg );
                *outmsg += "{\"accepted\":true}";
            } else {
                *outmsg += "{\"accepted\":false,\"msg\":\"Not all mandatory arguments passed\"}"; 
            }
        } catch ( std::out_of_range ) {
            *outmsg += "{\"accepted\":false,\"msg\":\"No such handler registered for this type of handler\"}";
        }
    } else if ( rcv == Receiver::proxy ) {
        try {
            ProxyRouteItemInfo_t *item = &proxy_callbacks.at(requested_handler_by_uri);
            if ( areArgsOk( item->args, params() )) {
                item->cb( parser.params(), outmsg );
            } else {
                *outmsg += "{\"accepted\":false,\"msg\":\"Not all mandatory arguments passed\"}"; 
            }
        } catch ( std::out_of_range ) {
            *outmsg += "{\"accepted\":false,\"msg\":\"No such handler registered for this type of handler\"}";
        }
    }
}

bool 
RouteManager::isAccepted(const string &uri) {
    return true;
}

bool 
RouteManager::isValidURI( const string &uri ) {
    return isAccepted(uri);
}

void
addDeviceCallback( const char *path, const URLParams_t &params, CallbackDevice_t cb ) {
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
addProxyCallback( const char *path, const URLParams_t &params, CallbackProxy_t cb ) {
    try {
        device_callbacks.at(path);
    } catch (std::out_of_range) {
        proxy_callbacks[path] = { params, cb };
    }
}

void 
install_callbacks() {
    addProxyCallback( "/proxy/ping",                URLParams_t(), proxyapi::ping );
    addProxyCallback( "/proxy/uptime",              URLParams_t(), proxyapi::uptime );
    addDeviceCallback( "/device/ping",              URLParams_t({ "did" }), deviceapi::ping );
    addDeviceCallback( "/device/reset",             URLParams_t({ "did" }), deviceapi::reset );
    addDeviceCallback( "/device/uptime",            URLParams_t({ "did" }), deviceapi::uptime );
    addDeviceCallback( "/device/pin/set-in",        URLParams_t({ "did" }), deviceapi::setPinAsInput );
    addDeviceCallback( "/device/pin/set-out",       URLParams_t({ "did" }), deviceapi::setPinAsOutput );
    addDeviceCallback( "/device/pin/value/set0",    URLParams_t({ "did" }), deviceapi::setPinLow );
    addDeviceCallback( "/device/pin/value/set1",    URLParams_t({ "did" }), deviceapi::setPinHigh );
    addDeviceCallback( "/device/pin/getvalue",      URLParams_t({ "did" }), deviceapi::getPinValue );
    addDeviceCallback( "/device/eeprom/read",       URLParams_t({ "did", "address" }), deviceapi::readEEPROM );
    addDeviceCallback( "/device/eeprom/write",      URLParams_t({ "did", "address", "value" }), deviceapi::writeEEPROM );


}


