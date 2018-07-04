#include <sys/sysinfo.h>
#include "proxy-api.h"

namespace proxyapi { 

void dummy(const UserArgs_t &args, string *output) {

}

void uptime(const UserArgs_t &args, string *output) {
    struct sysinfo info;
    int rc = sysinfo(&info);
    *output += "{\"uptime\":\"";
    if ( rc == 0 ) {
        *output += info.uptime;
    } else {
        *output += "no data";
    }
    *output += "\"}";
};

void ping(const UserArgs_t &args, string *output) {
    *output += "{\"ping\":\"pong\"}";
};


}; // namespace proxyapi

