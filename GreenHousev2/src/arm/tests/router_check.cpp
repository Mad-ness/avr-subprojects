#include <iostream>
// #include <regex>
#include <string>
#include "../routemanager.h"

using namespace std;

int main() {

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
/*
        if ( regex_match(input, integer))
            cout << "integer" << endl;
        else
            cout << "Invalid input" << endl;
*/
        std::cout << std::endl;
    }

    return 0;
}

