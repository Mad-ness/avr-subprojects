#ifndef __PROXY_API_H__
#define __PROXY_API_H__

#include <routemanager.h>

namespace proxyapi {

    void uptime(UserArgs_t &args, string *output);
    void dummy(UserArgs_t &args, string *output);

};

#endif // __PROXY_API_H__

