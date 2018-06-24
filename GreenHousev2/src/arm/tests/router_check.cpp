#include <iostream>
#include <regex>
#include <string>
#include "../routers.h"

using namespace std;

int main() {

    string input;
    regex integer("(\\+|-)?[[:digit:]]+");
    while ( true ) {
        cout << "Type an integer: ";
        cin >> input;
        if ( !cin ) break;
        if ( input == "q" ) break;
        if ( regex_match(input, integer))
            cout << "integer" << endl;
        else
            cout << "Invalid input" << endl;
    }

    return 0;
}

