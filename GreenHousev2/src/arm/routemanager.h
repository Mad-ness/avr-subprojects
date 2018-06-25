#ifndef __ROUTEMANAGER_H__
#define __ROUTEMANAGER_H__

#include <string>
#include <unordered_map>
#include <vector>
#include <ghairdefs.h>
#include <yuarel.h>
//#include <EdUrlParser.h>

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
typedef vector<string> Folder_t;
struct yuarel;

class UrlManager {
    private:
        KeyValueMap_t m_keysvalues;
        Folder_t folders;
        struct yuarel m_url;
//        struct yuarel_param *params;
    public:
        ~UrlManager();
        bool parse(const string url);
        bool parse2(const char *uri);
        Folder_t &paths() { return folders; };
        KeyValueMap_t &params() { return m_keysvalues; };
        KeyValueMap_t &args();
        void print(char splitter='\n');
};

#endif // __ROUTEMANAGER_H__

