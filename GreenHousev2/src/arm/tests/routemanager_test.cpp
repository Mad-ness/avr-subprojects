#include <iostream>
#include <cassert>
#include <ghairdefs.h>
#include <surlparser.h>
#include <routemanager.h>


void test_RouteInfo() {

    RouteManager m;
    assert( m.isValidURI( "/proxy/ping" ));
    assert( m.isValidURI( "////proxy/////ping////?" ) && m.path() == "/proxy/ping" );
    assert( m.isValidURI( "/proxy/uptime?did=10" ));
    assert( ! m.isValidURI( "/device/ping" ));
    std::cout << m.emsg() << std::endl;
    assert( m.isValidURI( "/device/ping?did=1" ));
    assert( m.isValidURI( "/device/uptime?did=2" ));
    assert( ! m.isValidURI( "/device/uptime?deviceid=2" ));
    assert( m.isValidURI( "/device/uptime?deviceid=2&did=17&addr=19" ));
    assert( m.isValidURI( "/device/setee?did=17&addr=19&value=17" ));
    assert( !  m.isValidURI( "/device/setee?deviceid=17&address=19&val=17" ));
    std::cout << m.emsg() << std::endl;
    assert( ! m.isValidURI( "/device/setee?address=19&value=17&did=6" ) && m.params().size() >= 3 );
    std::cout << m.emsg() << std::endl;
    assert( m.isValidURI( "//////device/setee////?addr=19&value=17&did=6" ));
    std::cout << " === Route info assertions passed ===\n";

}

int main() {

    test_RouteInfo();
    return 0;
}

