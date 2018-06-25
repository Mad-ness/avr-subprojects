#include <iostream>
#include <regex>
#include <routemanager.h>
//#include "surlparser.h"

/**
 * Returns a code of function "func" for passing to the remote
 * board and a list of arguments for the function.
 * Accepts a raw URI string in format
 * http://example.com/[path/path/path/../path?arg1=val1&arg2=val2&...]
 * the part in the square brackets should be passed in "uri".
 */
bool 
RouteManager::parseURI(const string &uri, int *func) {
    std:smatch m;
    for ( auto &n : Routers ) {
        std::regex r(n.first);
        if ( std::regex_match(uri, m, r) ) {
            *func = n.second; 
            return true;
        }
    }
    return false;
};


/**
 * It accepts a URI, normalize to the path component only
 * and compares with predefined sets in SimpleRouters.
 * It returns true if matching is found
 * The "func" contains a code of the user function
 * defined in the SimpleRouters array. 
 * "func" == -1 if a match not found.
 */
bool RouteManager::findCmd(const string &uri, int *func) {
    bool result = false;
    *func = -1;
    string s = SUrlParser(uri.c_str()).path();
    for ( int i = 0; i < sizeof(SimpleRouters)/sizeof(KeyValuePair_t); i++ ) {
        KeyValuePair_t pair = SimpleRouters[i];
        if ( pair.first == s ) {
            *func = pair.second;
            result = true;
            break;
        }
    }
    return result;
}

bool
RouteManager::findParams(const string &uri, KeyValueMap_t *params) {
    SUrlParser parser;
    if ( parser.parse( uri.c_str() )) { 
        *params = KeyValueMap_t(parser.params());
        return true;
    }
    return false;
}

