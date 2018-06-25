#include <routemanager.h>
#include <iostream>
#include <cassert>
#include <ghairdefs.h>
#include <surlparser.h>



void test_detectCmd() {
    int func;
    assert( RouteManager::findCmd( "/device/ping", &func) && func == AIR_CMD_IN_PING );
    assert( RouteManager::findCmd( "device/ping", &func)  && func == AIR_CMD_IN_PING );
    assert( RouteManager::findCmd( "/device////ping////", &func) && func == AIR_CMD_IN_PING );
    assert( RouteManager::findCmd( "/device/setee?did=1&addr=0&v=17", &func) && func == AIR_CMD_IN_WRITE_EEPROM );
    assert( RouteManager::findCmd( "/device/uptime?ask=now", &func) && func == AIR_CMD_IN_UPTIME );
    std::cout << "findCmd() test passed" << std::endl;
}

void test_findParams() {
    KeyValueMap_t params;
    assert( RouteManager::findParams( "/device/ping", &params ) && params.size() == 0 );
    assert( RouteManager::findParams( "/device/ping?did=1", &params ) && params.size() == 1 && params["did"] == "1" );
    assert( RouteManager::findParams( "/device/getee?did=1&addr=15", &params ) && params.size() == 2 && params["did"] == "1" && params["addr"] == "15" );
    assert( RouteManager::findParams( "/device/setee?did=1&addr=15&v=17", &params ) && params.size() == 3 && params["did"] == "1" && params["addr"] == "15" && params["v"] == "17" );
    assert( RouteManager::findParams( "/device/uptime?arg1&arg2&arg3=000&arg4&arg5", &params ) && params.size() == 5 && params["arg4"] == "" );

    std::cout << "findParams() test passed" << std::endl;
}

int main() {

    test_detectCmd();
    test_findParams();

    return 0;
}

