//#include <iostream>
// #include <regex>
#include <string>
#include <cassert>
#include "../routemanager.h"
//#include <yuarel.h>

using namespace std;

int main(int argc, char **argv) {
    if ( argc < 2 ) {
        printf("Provide an url for parsing.\n");
        return 0;
    }
    
    UrlManager m;
    m.parse2(argv[1]);

    m.parse2("/one/two/three");
    assert( m.paths().size() == 3 && m.params().size() == 0 );
    m.parse2("//one/two/three");
    assert( m.paths().size() == 3 && m.params().size() == 0 );
    m.parse2("//one//two/three");
    assert( m.paths().size() == 3 && m.params().size() == 0 );
    m.parse2("//one//two//three/four////five/six");
    assert( m.paths().size() == 6 && m.params().size() == 0 );
    m.parse2("//one//two//three/four////five/six/");
    assert( m.paths().size() == 6 && m.params().size() == 0 );
    m.parse2("//one//two//three/four////five/six////seven///////");
    assert( m.paths().size() == 7 && m.params().size() == 0 );


    m.parse2("/one/two?");
    assert( m.paths().size() == 2 && m.params().size() == 0 );
    m.parse2("/one/two//?");
    assert( m.paths().size() == 2 && m.params().size() == 0 );
    m.parse2("/one/two////?");
    assert( m.paths().size() == 2 && m.params().size() == 0 );

    m.parse2("/one/two?arg");
    assert( m.paths().size() == 2 && m.params().size() == 1 );
    m.parse2("/one/two//?arg=10");
    assert( m.paths().size() == 2 && m.params().size() == 1 );
    m.parse2("/one/two//?arg=10=");
    assert( m.paths().size() == 2 && m.params().size() == 1 );
    m.parse2("/one/two//?arg=====");
    assert( m.paths().size() == 2 && m.params().size() == 1 );
    m.parse2("//////one/////two//////?arg=10%323sdfs");
    assert( m.paths().size() == 2 && m.params().size() == 1 );


    m.parse2("/one/two?arg1&arg2");
    assert( m.paths().size() == 2 && m.params().size() == 2 );

    m.parse2("/one/two?arg1=10&arg2");
    assert( m.paths().size() == 2 && m.params().size() == 2 );

    m.parse2("/one/two?arg1=10&arg2=20");
    assert( m.paths().size() == 2 && m.params().size() == 2 );
    m.parse2("/one/two////?arg1&arg2=20");
    assert( m.paths().size() == 2 && m.params().size() == 2 );
    m.parse2("/one/two?arg1====&arg2");
    assert( m.paths().size() == 2 && m.params().size() == 2 );
    m.parse2("/one/two/?arg1====&arg2====");
    assert( m.paths().size() == 2 && m.params().size() == 2 );
    m.parse2("/one/two/?arg1=???&arg2=%%%&arg3=@@super-baby");
    assert( m.paths().size() == 2 && m.params().size() == 3 );
 

    m.parse2("?arg1=1&arg2=2&arg3=3");
    assert( m.paths().size() == 0 && m.params().size() == 3 );
    m.parse2("/?arg1=1&arg2=2&arg3=3");
    assert( m.paths().size() == 0 && m.params().size() == 3 );
    m.parse2("/////?arg1=1&arg2=2&arg3=3");
    assert( m.paths().size() == 0 && m.params().size() == 3 );
    m.parse2("arg1=1&arg2=2&arg3=3");
    assert( m.paths().size() == 0 && m.params().size() == 3 );
    m.parse2("arg1&arg2=2&arg3");
    assert( m.paths().size() == 0 && m.params().size() == 3 );
    m.parse2("arg1&arg2&arg3=3");
    assert( m.paths().size() == 0 && m.params().size() == 3 );
    m.parse2("arg1&arg2&arg3");
    assert( m.paths().size() == 0 && m.params().size() == 3 );

    m.parse2("arg1&arg2=U3VwZXJiYWJ5Cg==&arg3");
    assert( m.paths().size() == 0 && m.params().size() == 3 );

    m.parse2("one/two/three/////");
    assert( m.paths().size() == 3 && m.params().size() == 0 );




    return 0;
}

/*
int main2() {

    string input;
    RouteManager m;
    // regex integer("(\\+|-)?[[:digit:]]+");
    while ( true ) {
        cout << "Type URI arguments http://example.com?<your args>: ";
        cin >> input;
        if ( !cin ) break;
        if ( input == "q" ) break;

        m.parse(input);
        m.print('\n');
        if ( regex_match(input, integer))
            cout << "integer" << endl;
        else
            cout << "Invalid input" << endl;
        std::cout << std::endl;
    }

    return 0;
}
*/
