#include "requestmanager.h"
#include <string>


string
makeSHA256(const char *uri) {
    SHA256_CTX ctx;
    BYTE buf[SHA256_BLOCK_SIZE];
    sha256_init(&ctx);
    sha256_update(&ctx, (const BYTE*)uri, strlen(uri));
    sha256_final(&ctx, buf);
    return string((const char*)buf);
}


bool
RequestManager::addRequest(const char *uri, string *output) {
    if ( m_parser.parse(uri) ) {
        

    } else {
        *output += "{\"accepted\":false,\"msg\":\"Error parsing of the URI\"}";
    }
}

