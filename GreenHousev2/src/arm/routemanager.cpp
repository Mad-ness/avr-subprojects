#include <iostream>
#include <unordered_map>
#include <routemanager.h>
#include <proxy-api.h>
#include <device-api.h>
#include <string.h>
#include <time.h>
#include <crypto/sha256.h>
#include <ghair.h>


static DeviceCallbacksList_t device_callbacks;
static ProxyCallbacksList_t proxy_callbacks;


/*
RouteItemsInfo_t RouteItemsInfo[] = {                                            // this array mustn't be empty{
    { "dummy",                              URLParams_t(), proxyapi::dummy },    // keep this record always here
    { "/device/ping",                       URLParams_t(), deviceapi::ping },
    { "/device/pin/value/set",              URLParams_t({ "did", "pid" }), deviceapi::setPinInput },
    { "/proxy/ping",                        URLParams_t() },
    { "/proxy/uptime",                      URLParams_t() },
    { "/device/reset",                      URLParams_t({ "did" } )},
    { "/device/uptime",                     URLParams_t({ "did" } )},
    { "/device/getee",                      URLParams_t({ "did", "addr" } )},
    { "/device/setee",                      URLParams_t({ "did", "addr", "value" } )},
    { "/device/pin/set-in",                 URLParams_t({ "did", "pid" } )},
    { "/device/pin/set-out",                URLParams_t({ "did", "pid" } )},
    { "/device/pin/value",                  URLParams_t({ "did", "pid" } )},
    { "/device/pin/value/set0",             URLParams_t({ "did", "pid" } )},
    { "/device/pin/value/set1",             URLParams_t({ "did", "pid" } )},
};
*/

string
makeSHA256(const char *uri) {
    SHA256_CTX ctx;
    BYTE buf[SHA256_BLOCK_SIZE];
    sha256_init(&ctx);
    sha256_update(&ctx, (const BYTE*)uri, strlen(uri));
    sha256_final(&ctx, buf);
    string s;
    char buf2[4];
    for ( int i=0; i<SHA256_BLOCK_SIZE; i++ ) {
        sprintf( buf2, "%02x", buf[i] );
        s += buf2;
    }
    return string(s);
}


bool 
areArgsOk( const URLParams_t &mandatory_args, const KeyValueMap_t &passed_args) {
    for ( auto &arg : mandatory_args ) {
        if ( passed_args.find(arg) == passed_args.end() )
            return false;   // seek for the end and didn't find the entry
    }
    return true;
}


/**
 * It implements the air.loop() functionality
 * Call this method as often as possible to not miss
 * responses on your requests
 */
void
RouteManager::processResponses() {

    if ( air->receivedPacket() ) { // check if it's got new data and processing required
        const AirPacket &pkt = air->packet();
        for ( auto &request_full : m_requests ) {
            RequestItem_t &req = request_full.second;
            if ( req.hasSent() && req.packet_id == pkt.request_id ) {
                req.when.completed = time(NULL);
                req.status = RequestStatus::Completed;
                break;
            }
        }
    }

};


RequestItem_t*
RouteManager::addRequestInQueue(const char *uri) {
    const string url_hash = makeSHA256(uri);
    RequestItem_t *item = nullptr;
    try {
       item = &m_requests.at(url_hash);
       item->num_requests++;
    } catch ( std::out_of_range ) {
        int last_package_id = 0;
        if ( m_requests.size() > 0 ) {
            for ( auto &p : m_requests ) {
                if ( p.second.packet_id > last_package_id ) {
                    last_package_id = p.second.packet_id;
                }
            }
        }
        m_requests.insert({ url_hash, RequestItem_t() });
        item = &m_requests.at(url_hash);
        item->path = parser.path();
        item->args = parser.params();
        item->when.received = time(NULL); 
        item->when.completed = 0;
        item->when.scheduled = 0;
        item->uri_hash = makeSHA256(uri);
        item->num_requests = 1;
        item->packet_id = ++last_package_id;
        item->status = RequestStatus::New;
    }
    return item;
}

string
userArgsToStr(const UserArgs_t &args) {
    string result;
    for ( auto &n : args ) {
        result += n.first;
        result += "=";
        result += n.second;
        result += ",";
    }
    result.pop_back(); // remove last ","
    return string(result);
}

void 
RouteManager::printInQueue() {
    for ( auto &n : m_requests ) {
        std::cout << n.first
                  << " queries=" << n.second.num_requests
                  << " url=" << n.second.path
                  << " args=" << userArgsToStr(n.second.args)
                  << "\n";
    }
}

CallbackDevice_t&
getDeviceCallbackByURI(const string &path) {
    CallbackDevice_t *cb_ptr = nullptr;
    for ( auto &cb : device_callbacks ) {
        if ( cb.first == path ) {
            cb_ptr = &cb.second.cb;
        }
    }
    return *cb_ptr;
}

void
RouteManager::loop() {
    processRequestsQueue();
}

void
RouteManager::processRequestsQueue() {
    request_time_t curr_time = time(NULL);
    RequestItem_t *item = nullptr;
    for ( auto &request : m_requests ) {
        if ( ! request.second.hasSent() ) {
            
            // select this if the request has not been sent out yet
            if ( request.second.when.scheduled == 0 ) {
                item = &request.second;
                break;

            // select this if the request's wait time is over than 1 second
            } else if ( curr_time - request.second.when.scheduled >= 1 ) {
                item = &request.second;
                break;
            }

        } else {
            if ( request.second.when.completed > 0 && curr_time - request.second.when.completed > 5 ) {
                m_requests.erase( request.second.uri_hash );
            }
        }
    }

    if ( item != nullptr ) {
        CallbackDevice_t &cb = getDeviceCallbackByURI(item->path);
        string output;
        if ( air->rf24()->isChipConnected() ) {
            air->setPacketId( item->packet_id );
            if ( cb( air, item->args, &output )) {
                item->when.scheduled = time(NULL); 
                item->done_attempts++;
                // item->has_sent = true;
                item->status = RequestStatus::Sent;
                item->errmsg.clear();
                std::cout << "Request "
                          << item->path
                          << ", has been sent out\n";
            } else {
                item->failed_attempts++;
                item->errmsg = output;
                std::cout << "Request "
                          << item->path
                          << ", has not been sent out\n";
            }
        } else {
            item->errmsg = "Wireless module not connected";
        }
    }
}

void
RouteManager::accept(const char *uri, string *outmsg) {
    if ( parser.parse(uri) ) {
        // std::cout << "DEBUG: starting parsing\n";
        if ( strncmp( "/device", uri, 7 ) == 0 ) {
            callHandler( uri, Receiver::device, outmsg );
        } else if ( strncmp( "/proxy", uri, 6 ) == 0 ) {
            callHandler( uri, Receiver::proxy, outmsg );
        } else {
            *outmsg += "{\"accepted\":false,\"msg\":\"Not supported handler type\"}";
        }
        // std::cout << "DEBUG: URI has been parsed\n";
    } else {
        *outmsg += "{\"accepted\":false,\"msg\":\"Cannot parse the url\"}";
    }
}

void
RouteManager::callHandler(const char *uri, const Receiver rcv, string *outmsg) {
    const string requested_handler_by_uri = parser.path();
    if ( rcv == Receiver::device ) {
        try {
            DeviceRouteItemInfo_t *item = &device_callbacks.at(requested_handler_by_uri);
            if ( areArgsOk( item->args, params() )) {


                // item->cb( &air, parser.params(), outmsg );
                RequestItem_t *item = addRequestInQueue(uri);

                *outmsg += "{\"accepted\":true,\"next\":\"/byhash/";
                *outmsg += item->uri_hash;
                *outmsg += "\"}";
            } else {
                *outmsg += "{\"accepted\":false,\"msg\":\"Not all mandatory arguments passed\"}"; 
            }
        } catch ( std::out_of_range ) {
            *outmsg += "{\"accepted\":false,\"msg\":\"No such handler registered for this type of handler\"}";
        }
    } else if ( rcv == Receiver::proxy ) {
        try {
            ProxyRouteItemInfo_t *item = &proxy_callbacks.at(requested_handler_by_uri);
            if ( areArgsOk( item->args, params() )) {

                item->cb( parser.params(), outmsg );


            } else {
                *outmsg += "{\"accepted\":false,\"msg\":\"Not all mandatory arguments passed\"}"; 
            }
        } catch ( std::out_of_range ) {
            *outmsg += "{\"accepted\":false,\"msg\":\"No such handler registered for this type of handler\"}";
        }
    }
}

bool 
RouteManager::isAccepted(const string &uri) {
    return true;
}

bool 
RouteManager::isValidURI( const string &uri ) {
    return isAccepted(uri);
}

void
addDeviceCallback( const char *path, const URLParams_t &params, CallbackDevice_t cb ) {
    try {
        proxy_callbacks.at(path); 
    } catch (std::out_of_range) {
        device_callbacks[path] = { params, cb };
    }
}

void 
printHandlers() {
    for ( auto &handler : proxy_callbacks ) {
        std::cout << handler.first << std::endl;
    }
    for ( auto &handler : device_callbacks ) {
        std::cout << handler.first << std::endl;
    }
}

void
addProxyCallback( const char *path, const URLParams_t &params, CallbackProxy_t cb ) {
    try {
        device_callbacks.at(path);
    } catch (std::out_of_range) {
        proxy_callbacks[path] = { params, cb };
    }
}


void 
install_callbacks() {

    /**
     * The params that started with the question mark "?"
     * are the parameters which we'll be returned. They should 
     * not be interpretered as the incoming (mandatory) parameters.
     * The name after such param indicates the type of data of the param.
     * The name of such param should not overlap with existing names.
     */

    addProxyCallback( "/proxy/ping",                URLParams_t(), proxyapi::ping );
    addProxyCallback( "/proxy/uptime",              URLParams_t(), proxyapi::uptime );
    addDeviceCallback( "/device/ping",              URLParams_t({ "did" }), deviceapi::ping );
    addDeviceCallback( "/device/reset",             URLParams_t({ "did" }), deviceapi::reset );
    addDeviceCallback( "/device/uptime",            URLParams_t({ "did" }), deviceapi::uptime );
    addDeviceCallback( "/device/pin/set-in",        URLParams_t({ "did" }), deviceapi::setPinAsInput );
    addDeviceCallback( "/device/pin/set-out",       URLParams_t({ "did" }), deviceapi::setPinAsOutput );
    addDeviceCallback( "/device/pin/value/set0",    URLParams_t({ "did" }), deviceapi::setPinLow );
    addDeviceCallback( "/device/pin/value/set1",    URLParams_t({ "did" }), deviceapi::setPinHigh );
    addDeviceCallback( "/device/pin/getvalue",      URLParams_t({ "did", "?value:int8" }), deviceapi::getPinValue );
    addDeviceCallback( "/device/eeprom/read",       URLParams_t({ "did", "address", "?value:int8" }), deviceapi::readEEPROM );
    addDeviceCallback( "/device/eeprom/write",      URLParams_t({ "did", "address", "value", "?writtenvalue:int8" }), deviceapi::writeEEPROM );

}


