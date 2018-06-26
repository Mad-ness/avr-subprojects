#include <iostream>
#include <routemanager.h>
//#include "surlparser.h"



RouteItemsInfo_t RouteItemsInfo[] = {                                     // this array mustn't be empty{
    { "dummy",              0x0,                            URLParams_t() },    // keep this record always here
    { "/proxy/ping",        0x1,                            URLParams_t() },
    { "/proxy/uptime",      0x2,                            URLParams_t() },
    { "/device/ping",       AIR_CMD_IN_PING,                URLParams_t({ "did" } )},
    { "/device/reset",      AIR_CMD_IN_RESET,               URLParams_t({ "did" } )},
    { "/device/uptime",     AIR_CMD_IN_UPTIME,              URLParams_t({ "did" } )},
    { "/device/getee",      AIR_CMD_IN_GET_EEPROM,          URLParams_t({ "did", "addr" } )},
    { "/device/setee",      AIR_CMD_IN_WRITE_EEPROM,        URLParams_t({ "did", "addr", "value" } )},
};

bool 
RouteManager::isAccepted(const string &uri) {
    bool result = false;
    if ( parser.parse(uri.c_str()) ) {
        for ( int i = 0; i < sizeof(RouteItemsInfo)/sizeof(RouteItemsInfo_t); i++ ) {
    
            RouteItemsInfo_t *item = &RouteItemsInfo[i];
            const uint8_t cmd = item->hasFoundCmd(parser.path());
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
                }
            }
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

