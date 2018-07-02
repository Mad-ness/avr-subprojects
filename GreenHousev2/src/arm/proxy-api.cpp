#include <sys/sysinfo.h>
#include "proxy-api.h"

namespace proxyapi { 

void dummy(UserArgs_t &args, string *output) {

}

void uptime(UserArgs_t &args, string *output) {
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


}; // namespace proxyapi

