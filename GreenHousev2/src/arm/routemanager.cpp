#include <iostream>
#include "routemanager.h"


void RouteManager::parse(string uri) {
    m_keysvalues.clear();
    size_t sep_pos = uri.find_first_of("?");
    string folders = uri.substr(0, sep_pos);
    string args = uri.substr(sep_pos+1, uri.length() - sep_pos);
    size_t elements = EdUrlParser::parseKeyValueMap(&m_keysvalues, args, true);
    EdUrlParser::parsePath(&m_folders, folders);
}

KeyValueMap_t &RouteManager::args() {
    return m_keysvalues;
}

Folders_t &RouteManager::folders() {
    return m_folders;
}

void RouteManager::print(char splitter) {
    for ( auto &n : m_folders ) {
        std::cout << n << " / ";
    }
    std::cout << std::endl;
    for ( const auto &n : m_keysvalues ) {
        std::cout << "Key:[" << n.first << "] Value: [" << n.second << "]" << splitter;
    }
}

