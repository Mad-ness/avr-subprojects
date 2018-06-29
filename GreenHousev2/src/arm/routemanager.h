#pragma once
#ifndef __ROUTEMANAGER_H__
#define __ROUTEMANAGER_H__

#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <unordered_map>
#include <ghairdefs.h>
#include <surlparser.h>

#define ERR_INVALID_CMD         (0xFF)
#define ERR_INVALID_PARAMS      (0xFE)

using namespace std;

typedef vector<string> URLParams_t;
typedef tuple<string, int, URLParams_t> RouterInfo_t;

struct RouteItemsInfo_t {
    const char *path; // all paths must be in lowercase
    uint8_t cmd;
    URLParams_t args;
    /**
      * Checking if all required arguments were passing.
      * @passed_args - a list of arguments that has come for checking
      * @missed_args - a list of arguments that missing
      * @return False if all is Ok.
      */
    bool missedArgs(const KeyValueMap_t &passed_args, URLParams_t *missed_args) {
        bool result = false;
        for ( auto &a : args ) {
            try {
                passed_args.at(a);  // checking if a required argument among the passed_args
            } catch ( std::out_of_range ) {
                missed_args->push_back(a);
                result = true;
            }
        }
        return result;
    }
    /**
      * Returns a code of low-level (AirProto-level) function if 
      * the passed URI matches that records.
      * Returns ERR_INVALID_CMD if it doesn't match
      */
    uint8_t hasFoundCmd(const string &uri) {
        string s1(uri);
        std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
        return ( string(path) == s1 ) ? cmd : ERR_INVALID_CMD;
    }
};


class RouteManager {
    private:
        enum class CallRC { inqueue, response }; // wether a call is queued or is an immediate response
        SUrlParser parser;
        string errmsg;
        bool isAccepted(const string &uri);
        void callAPI(const RouteItemsInfo_t &apifunc, RouteManager::CallRC *whatsdone);
    public:
        const string &emsg() { return errmsg; };
        bool isValidURI(const string &uri); 
        string path() { return parser.path(); };
        KeyValueMap_t params() { return parser.params(); }
};

#endif // __ROUTEMANAGER_H__

