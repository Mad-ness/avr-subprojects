#include <iostream>
#include <string>
//#include <yuarel.h>
#include "routemanager.h"

UrlManager::~UrlManager() {
}

bool UrlManager::parse2(const char *uri) {

    folders.clear();
    m_keysvalues.clear();
    enum Type { begin, folder, param, value };
    string s;

    std::cout << "Origin URI: " << uri << std::endl;
    const char *c = uri;
    // scan the whole string
    enum Type t = Type::begin;
    string last_param;
    if ( c != NULL || *c != '\0' ) {
        do {
            if ( *c == '/' ) {
                if ( s.length() > 0 ) {
                    folders.push_back(s);
                    s.clear();
                }
                t = Type::folder;
            } else if ( *c == '?' && ( t == Type::folder || t == Type::begin )) {
                if ( s.length() > 0 && s.length() > 0 ) {
                    folders.push_back(s);
                    s.clear();
                }
                t = Type::param;
            } else if ( *c == '=' && ( t == Type::param || t == Type::begin )) {
                m_keysvalues[s] = "";
                last_param = s;
                s.clear();
                t = Type::value;
            } else if ( *c == '&' && ( t == Type::value || t == Type::param || t == Type::begin )) {
                if ( t == Type::value ) {
                    m_keysvalues[last_param] = s;
                } else if (( t == Type::param || t == Type::begin ) && ( s.length() > 0 )) {
                    m_keysvalues[s] = "";
                    last_param = s;
                }
                t = Type::param;
                s.clear(); 
            } else if ( *c == '\0' && s.length() > 0 ) {
                if ( t == Type::value ) {
                    m_keysvalues[last_param] = s;
                } else if ( t == Type::folder ) {
                    folders.push_back(s);
                } else if ( t == Type::param ) {
                    m_keysvalues[s] = "";
                }
                s.clear();
            } else if ( *c == '\0' && s.length() == 0 ) {
                if ( t == Type::param && last_param.length() > 0 ) {
                    m_keysvalues[last_param] = "";
                }
                s.clear();
            } else { 
                s += *c;
            }
        } while ( *c++ != '\0' );
    }
    std::cout << "Found folders " << folders.size() << ", are:" << std::endl;
    for ( auto &n : folders) {
        std::cout << " - " << n << std::endl;
    }
    print(); 
    return true;
}

bool UrlManager::parse(const string url)
{
    m_keysvalues.clear();
    char *cstr = new char[url.length() + 1];
    strcpy( cstr, url.c_str() );

    if (-1 == yuarel_parse(&m_url, cstr)) {
        return false;
    }
  
    fprintf(stdout, "Incoming str accepted: %s\n", url.c_str());
    fprintf(stdout, "Query str: %s = %s\n", m_url.query, cstr);
    delete[] cstr;

    int p = 0;
    int num_args = 0;
    if ( strlen( m_url.query ) > 0 ) { 
        num_args = 1;
        char *c = m_url.query;
        while ( *c++ != '\0' ) {
            if ( *c == '&' ) 
                num_args++;
        }
        printf("Detected probably %d params\n", num_args);
        struct yuarel_param *params = new yuarel_param[num_args];
        int p = yuarel_parse_query(m_url.query, '&', params, num_args);
        printf("Params parsed, total we've got %d\n", p);
        while ( p-- > 0 ) {
            m_keysvalues[params[p].key] = params[p].val;
        }
        delete[] params;
    }
    return true;
}

KeyValueMap_t &UrlManager::args() {
    return m_keysvalues;
}

void UrlManager::print(char splitter) {
    std::cout << std::endl;
    for ( const auto &n : m_keysvalues ) {
        std::cout << "Key:[" << n.first << "] Value: [" << n.second << "]" << splitter;
    }
}

