#include <iostream>
#include <routemanager.h>
#include <proxy-api.h>
#include <string.h>
//#include "surlparser.h"

#define PROXY_CMD_PING          200 // need to verify that ID, they mustn't cross with Device IDs
#define PROXY_CMD_UPTIME        201

RouteItemsInfo_t RouteItemsInfo[] = {                                     // this array mustn't be empty{
    { "dummy",                  0x0,                            URLParams_t() },    // keep this record always here
    { "/proxy/ping",            PROXY_CMD_PING,                 URLParams_t() },
    { "/proxy/uptime",          PROXY_CMD_UPTIME,               URLParams_t() },
    { "/device/ping",           AIR_CMD_IN_PING,                URLParams_t({ "did" } )},
    { "/device/reset",          AIR_CMD_IN_RESET,               URLParams_t({ "did" } )},
    { "/device/uptime",         AIR_CMD_IN_UPTIME,              URLParams_t({ "did" } )},
    { "/device/getee",          AIR_CMD_IN_GET_EEPROM,          URLParams_t({ "did", "addr" } )},
    { "/device/setee",          AIR_CMD_IN_WRITE_EEPROM,        URLParams_t({ "did", "addr", "value" } )},
    { "/device/pin/set-in",     AIR_CMD_SET_PIN_MODE,           URLParams_t({ "did", "pid" })},
    { "/device/pin/set-out",    AIR_CMD_SET_PIN_MODE,           URLParams_t({ "did", "pid" })},
    { "/device/pin/value",      AIR_CMD_GET_PIN_VALUE,          URLParams_t({ "did", "pid" })},
    { "/device/pin/value/set0", AIR_CMD_SET_PIN_VALUE,          URLParams_t({ "did", "pid" })},
    { "/device/pin/value/set1", AIR_CMD_SET_PIN_VALUE,          URLParams_t({ "did", "pid" })},
};

/**
 * It calls a low-level API function in case if all checks passed
 *
 */
void RouteManager::callAPI(const RouteItemsInfo_t &apifunc, RouteManager::CallRC *whatsdone) {
    bool is_proxy_call = false;
    if ( strncmp(apifunc.path, "/proxy", strlen("/proxy")) == 0 ) {
        is_proxy_call = true; 
    }

    if ( is_proxy_call ) {
        switch ( apifunc.cmd ) {
            case AIR_CMD_IN_PING: {
                *whatsdone = CallRC::response;
                }; break;;
            case AIR_CMD_IN_UPTIME: {
                *whatsdone = CallRC::response;
                const long seconds = proxyapi::uptime();    
                }; break;;
        }
    } else {    // it's a device call
        switch ( apifunc.cmd ) {
            case AIR_CMD_IN_PING: {

                }; break;;
            case AIR_CMD_IN_UPTIME: {

                }; break;;
        };
    }
}

bool 
RouteManager::isAccepted(const string &uri) {
    bool result = false;
    if ( parser.parse(uri.c_str()) ) {
        uint8_t cmd = ERR_INVALID_CMD;
        for ( int i = 0; i < sizeof(RouteItemsInfo)/sizeof(RouteItemsInfo_t); i++ ) {
    
            RouteItemsInfo_t *item = &RouteItemsInfo[i];
            cmd = item->hasFoundCmd(parser.path());

            if ( cmd != ERR_INVALID_CMD ) {
                URLParams_t missed_params;
                if ( ! item->missedArgs(parser.params(), &missed_params ) ) {   
                    errmsg.clear();
                    result = true;
                    break;
                } else {
                    errmsg = "Passed Uri has missed some or all of the mandatory params: [";
                    for ( string &p : missed_params ) {
                        errmsg += p;
                        errmsg += " ";
                    }
                    errmsg.pop_back();  // remove last whitespace
                    errmsg += "]";
                    break;
                }
            }
        }
        if ( cmd == ERR_INVALID_CMD ) {
            errmsg = "Forbidden. Invalid command";
        }
    } else { 
        errmsg = "Passed Uri: ";
        errmsg += uri;
        errmsg += ", cannot be recognized as correct uri";
    }
    return result;
}

bool 
RouteManager::isValidURI(const string &uri) {
    return isAccepted(uri);
}

