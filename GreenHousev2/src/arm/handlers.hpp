#ifndef __URI_HANDLERS_H__
#define __URI_HANDLERS_H__

#include <routemanager.h>
#include <proxy-api.h>
#include <device-api.h>

REGISTER_DEVICE_CALLBACK( "/device/ping", { "did" }, deviceapi::ping );
REGISTER_DEVICE_CALLBACK( "/device/uptime", { "did" }, deviceapi::uptime );
REGISTER_PROXY_CALLABCK( "/proxy/ping", {}, proxyapi::ping );


#endif // __URI_HANDLERS_H__

