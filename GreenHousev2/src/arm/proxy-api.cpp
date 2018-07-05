#include <string.h>
#include <sys/sysinfo.h>
#include "proxy-api.h"

namespace proxyapi { 

void dummy(const UserArgs_t &args, string *output) {

}

void uptime(const UserArgs_t &args, string *output) {
    struct sysinfo info;
    int rc = sysinfo(&info);
    char n_str[15];
    *output += "{\"provided\":true,\"uptime\":";
    if ( rc == 0 ) {
        sprintf(n_str, "%lu", info.uptime);
        *output += n_str;
    } else {
        *output += "no data";
    }
    *output += "}";
};

void ping(const UserArgs_t &args, string *output) {
    *output += "{\"provided\":true,\"ping\":\"pong\"}";
};


}; // namespace proxyapi

