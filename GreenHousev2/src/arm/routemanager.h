#ifndef __ROUTEMANAGER_H__
#define __ROUTEMANAGER_H__

#include <string>
#include <unordered_map>
#include <ghairdefs.h>
#include <EdUrlParser.h>

using namespace std;

typedef std::pair<std::string, int> Router_t;

// Router_t one("Hello", 1);
/*
Router_t routers[] = {
    Router_t( "/proxy/ping$",                   0x0 ),
    Router_t( "/proxy/uptime$",                 0x0 ),
    Router_t( "/proxy/reboot$",                 0x0 ),
    Router_t( "/board/ping$",                   AIR_CMD_IN_PING ),
    Router_t( "/board/geteeprom?(.*)$",         AIR_CMD_IN_GET_EEPROM ),
    Router_t( "/board/seteeprom?(.*)$",         AIR_CMD_IN_WRITE_EEPROM )
};
*/

typedef unordered_map<string, string> KeyValueMap_t;
typedef vector<string> Folders_t;

class RouteManager {
    private:
        KeyValueMap_t m_keysvalues;
        Folders_t m_folders;
        string m_uri;
    public:
        void parse(string uri);
        KeyValueMap_t &args();
        Folders_t &folders();
        void print(char splitter='\n');
        

};

#endif // __ROUTEMANAGER_H__

