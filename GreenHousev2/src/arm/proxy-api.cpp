#include <sys/sysinfo.h>
#include "proxy-api.h"

namespace proxyapi { 

long uptime() {
    struct sysinfo info;
    int rc = sysinfo(&info);
    if ( rc == 0 ) {
        return info.uptime;
    }
    return -1;
};


}; // namespace proxyapi

