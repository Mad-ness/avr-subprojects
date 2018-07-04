#ifndef __PROXY_API_H__
#define __PROXY_API_H__

#include <routemanager.h>

namespace proxyapi {

    void ping(const UserArgs_t &args, string *output);
    void uptime(const UserArgs_t &args, string *output);
    void dummy(const UserArgs_t &args, string *output);

};

#endif // __PROXY_API_H__

