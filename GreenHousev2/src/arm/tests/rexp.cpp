#include <routemanager.h>
#include <cassert>
#include <ghairdefs.h>

int main(int argc, char **argv) {
    
    int func;

    assert( RouteManager::testparseURI( "/proxy/ping", &func ));
    assert( RouteManager::testparseURI( "/proxy/uptime", &func ));
    assert( RouteManager::testparseURI( "/proxy/reboot", &func ));
    assert( ! RouteManager::testparseURI( "/device//ping", &func ));
    assert( ! RouteManager::testparseURI( "/device/dasfadsfs/ping", &func ));
    assert( ! RouteManager::testparseURI( "/device/787df3/ping", &func ));

    assert( ! RouteManager::testparseURI( "/device/1/ping?arg1", &func ));
    assert( ! RouteManager::testparseURI( "/device/1/ping?arg1=1", &func ));
    assert( ! RouteManager::testparseURI( "/device/1/ping?arg1=1&arg2", &func ));
    assert( ! RouteManager::testparseURI( "/device/1/ping?arg1=1&arg2=2", &func ));
    assert( ! RouteManager::testparseURI( "/device/1/ping?arg1=1&arg2=2&arg3", &func ));
    assert( ! RouteManager::testparseURI( "/device/1/ping?arg1=1&arg2&arg3=3&arg4", &func ));

    assert( RouteManager::testparseURI( "/device/0/ping", &func ));
    assert( RouteManager::testparseURI( "/device/1234567890/ping", &func ));
    assert( RouteManager::testparseURI( "/device/032423/ping", &func ));
    
    assert( RouteManager::testparseURI( "/device/0/ping", &func ) && func == AIR_CMD_IN_PING );
    assert( RouteManager::testparseURI( "/device/1234/reset", &func ) && func == AIR_CMD_IN_RESET );
    assert( RouteManager::testparseURI( "/device/1/getee", &func ));
    assert( RouteManager::testparseURI( "/device/41/getee", &func ) && func == AIR_CMD_IN_GET_EEPROM );
    assert( ! RouteManager::testparseURI( "/device/d1/getee", &func ));
    assert( RouteManager::testparseURI( "/device/1/setee", &func ) && func == AIR_CMD_IN_WRITE_EEPROM );
    assert( ! RouteManager::testparseURI( "/proxy/1/setee", &func ));
 
    return 0;
}

