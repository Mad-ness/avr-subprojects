#ifndef __ROUTEMANAGER_H__
#define __ROUTEMANAGER_H__

#include <string>
//#include <regex>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <ghairdefs.h>
#include <surlparser.h>

using namespace std;

typedef std::pair<std::string, int> KeyValuePair_t;;
typedef std::unordered_map<string, string> Dict_t;


const KeyValuePair_t Routers[] = {
    { "/proxy/ping",                            0x0 },
    { "/proxy/uptime",                          0x0 },
    { "/proxy/reboot",                          0x0 },
    { "/device/[[:digit:]]+/ping",              AIR_CMD_IN_PING },
    { "/device/[[:digit:]]+/reset",             AIR_CMD_IN_RESET },
    { "/device/[[:digit:]]+/getee",             AIR_CMD_IN_GET_EEPROM },
    { "/device/[[:digit:]]+/setee",             AIR_CMD_IN_WRITE_EEPROM }
};


/**
 * /proxy/ping
 * /proxy/uptime
 * /device/ping/p/did/1                     == /device/ping?did=1
 * /device/reset/p/did/1                    == /device/ping?did=1
 * /device/uptime/p/did/1                   == /device/uptime?did=1
 * /device/time/p/did/1                     == /device/time?did=1
 * /device/getee/p/did/1?addr=0x0           == /device/getee?did=1&addr=0x0
 * /device/setee/p/did/1?addr=0x0&value=17  == /device/setee?did=1&addr=0x0&value=17
 *
 *
 * "URI path", { "FuncID", "list-params" }
 * "URI path", { "FuncID", { "param1", "param2", "param3" } }
 *
 */

typedef vector<string> URLParams_t;
typedef tuple<string, int, URLParams_t> RouterInfo_t;

const RouterInfo_t RoutersData[] = {
    std::make_tuple( "/proxy/ping",         0x1,                        URLParams_t() ),
    std::make_tuple( "/proxy/uptime",       0x2,                        URLParams_t() ),
    std::make_tuple( "/device/ping",        AIR_CMD_IN_PING,            URLParams_t({ "did" })),
    std::make_tuple( "/device/reset",       AIR_CMD_IN_RESET,           URLParams_t({ "did" })),
    std::make_tuple( "/device/uptime",      AIR_CMD_IN_UPTIME,          URLParams_t({ "did" })),
    std::make_tuple( "/device/getee",       AIR_CMD_IN_GET_EEPROM,      URLParams_t({ "did", "addr" })),
    std::make_tuple( "/device/setee",       AIR_CMD_IN_WRITE_EEPROM,    URLParams_t({ "did", "addr", "value" })),
};

const KeyValuePair_t SimpleRouters[] = {
    { "/proxy/ping",                        0x0 },
    { "/proxy/uptime",                      0x1 },
    { "/device/ping",                       AIR_CMD_IN_PING },
    { "/device/reset",                      AIR_CMD_IN_RESET },
    { "/device/getee",                      AIR_CMD_IN_GET_EEPROM },
    { "/device/setee",                      AIR_CMD_IN_WRITE_EEPROM },
    { "/device/uptime",                     AIR_CMD_IN_UPTIME },
};

class RouteManager {
    public:
        static bool parseURI(const string &uri, int *func);
        static bool findCmd(const string &uri, int *func);
        static bool findParams(const string &uri, KeyValueMap_t *params);
};

#endif // __ROUTEMANAGER_H__

