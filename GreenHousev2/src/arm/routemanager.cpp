#include <iostream>
#include <algorithm>
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

string
encloseQuotes(const string &str) {
    return '\"' + string( str ) + '\"';
}

string 
keyValueToStr( const string &key, const string &value ) {
    return encloseQuotes( key ) + ":" + encloseQuotes( value );
}


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


vector<string> split(string str, string token){
    vector<string>result;
    while(str.size()){
        int index = str.find(token);
        if(index!=string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
        }else{
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

void 
RouteManager::cleanUpRequestsQueue() {
    for ( auto &request_info : m_requests ) {
        RequestItem_t &req = request_info.second;
        request_time_t curr_time = time(NULL);
        // requests with responses older 5 seconds are removed
        if ( req.when.completed > 0 && curr_time - req.when.completed > 5 ) {
            std::cout << "Removed request: " << request_info.first << std::endl;
            m_requests.erase(request_info.first);
            break;
        }
    }
}

/**
 * It implements the air.loop() functionality
 * Call this method as often as possible to not miss
 * responses on your requests.
 * Returns a JSON string enclosed in the angle brackets the
 * returned string looks like this "{"name":"value",...}"
 */
void
RouteManager::handleResponses() {
    bool found_request = false;
    AirPacket *pkt = nullptr;
    RequestItem_t *req = nullptr;
    if ( air->receivedPacket() ) { // check if it's got new data and processing required
        pkt = &air->packet();
        if ( pkt->isResponse() ) {
            std::cout << "A response arrived\n";
            // looking for the record in the queue which has sent the request for this response
            for ( auto &request_full : m_requests ) {
                req = &request_full.second;
                if ( req->hasSent() && req->packet_id == pkt->request_id ) {
                    req->when.completed = time(NULL);
                    req->status = RequestStatus::Completed;
                    found_request = true;
                    break;
                }
            }
        }
    }

    string result;
    if ( found_request ) {
        // Seek for the handler which called this request "req"
        for ( auto &handler : device_callbacks ) {
            if ( handler.first == req->path ) {

                URLParams_t &output_params = handler.second.ret_args;
                // iterate over parameters defined in the callback
                // and upack them for extracting the parameters 
                // from the AirPacket(pkt).data
                int pos_shift = 0;
                std::sort( output_params.begin(), output_params.end() );
                for ( auto &arg : output_params ) {
                    vector<string> args_info = split(arg, ":");
                    string &data_type = args_info[2];
                    string &arg_name = args_info[1];

                    if ( pos_shift >= pkt->length ) {
                        break;
                    }
                    if ( data_type == "int8" ) {
                        int8_t value = 0;
                        memcpy( &value, &pkt->data[pos_shift], sizeof(int8_t) );
                        pos_shift += sizeof( int8_t );
                        req->args[arg_name] = std::to_string(value);
                    } else if ( data_type == "uint8") {
                        uint8_t value = 0;
                        memcpy( &value, &pkt->data[pos_shift], sizeof(uint8_t) );
                        pos_shift += sizeof( uint8_t );
                        req->args[arg_name] = std::to_string(value);
                    } else if ( data_type == "ulong" ) {
                        unsigned long value = 0;
                        memcpy( &value, &pkt->data[pos_shift], sizeof(unsigned long) );
                        pos_shift += sizeof( unsigned long );
                        req->args[arg_name] = std::to_string(value);
                    }
                } // for loop // iterate over output params
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
        string status = "unknown";
        switch ( n.second.status ) {
            case RequestStatus::New:
                status = "new";
                break;
            case RequestStatus::Sent:
                status = "sent";
                break;
            case RequestStatus::Completed: 
                status = "completed";
                break;
        }
        std::cout << n.first
                  << " queries=" << n.second.num_requests
                  << " url=" << n.second.path
                  << " args=" << userArgsToStr(n.second.args)
                  << " send_attempts=" << n.second.done_attempts
                  << " send_attempts_failed=" << n.second.failed_attempts
                  << " when.received=" << n.second.when.received 
                  << " when.scheduled=" << n.second.when.scheduled
                  << " when.completed=" << n.second.when.completed
                  << " status=" << status
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
    handleResponses();
    processRequestsQueue();
}

string 
RouteManager::getDetailsByHash(const string &hashsum) {
    string res = "";
    try {
        RequestItem_t &item = m_requests.at( hashsum ); 
        res += "{";
            res += keyValueToStr( "completed_at", std::to_string( item.when.completed ));
            res += ",";
            for ( auto &arg : item.args ) {
                res += keyValueToStr( arg.first, arg.second );
                res += ",";
            }
            res.pop_back();
        res += "}";
    } catch ( std::out_of_range ) {
        // res += encloseQuotes( );
        res += keyValueToStr( "msg", "No information found" );
        // res += "{\"msg\":\"No information found\"}";
    }
    return string(res);
}

void
RouteManager::processRequestsQueue() {
    cleanUpRequestsQueue();
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
addDeviceCallback( const char *path, const URLParams_t &params, const URLParams_t &r_params, CallbackDevice_t cb ) {
    try {
        proxy_callbacks.at(path); 
    } catch (std::out_of_range) {
        device_callbacks[path] = { params, r_params, cb };
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
addProxyCallback( const char *path, const URLParams_t &params, const URLParams_t &r_params, CallbackProxy_t cb ) {
    try {
        device_callbacks.at(path);
    } catch (std::out_of_range) {
        proxy_callbacks[path] = { params, r_params, cb };
    }
}


void 
install_callbacks() {

    /**
     * Format of ret_params.
     * - it is a vector of strings
     * - each of the strings has this format: <name>:<type>:<position>, where
     *   - name: name which will be returns to the end user in JSON response
     *   - type: type of data received from the remote module
     *   - position: position in the data file of the AirPacket
     * - supported types: int8, uint8
     */

    addProxyCallback( "/proxy/ping",                URLParams_t(), URLParams_t(), proxyapi::ping );
    addProxyCallback( "/proxy/uptime",              URLParams_t(), URLParams_t(), proxyapi::uptime );
    addDeviceCallback( "/device/ping",              URLParams_t({ "did" }), URLParams_t(), deviceapi::ping );
    addDeviceCallback( "/device/reset",             URLParams_t({ "did" }), URLParams_t(), deviceapi::reset );
    addDeviceCallback( "/device/uptime",            URLParams_t({ "did" }), URLParams_t(), deviceapi::uptime );
    addDeviceCallback( "/device/pin/set-in",        URLParams_t({ "did" }), URLParams_t(), deviceapi::setPinAsInput );
    addDeviceCallback( "/device/pin/set-out",       URLParams_t({ "did" }), URLParams_t(), deviceapi::setPinAsOutput );
    addDeviceCallback( "/device/pin/value/set0",    URLParams_t({ "did" }), URLParams_t(), deviceapi::setPinLow );
    addDeviceCallback( "/device/pin/value/set1",    URLParams_t({ "did" }), URLParams_t(), deviceapi::setPinHigh );
    addDeviceCallback( "/device/pin/getvalue",      URLParams_t({ "did" }), URLParams_t({ "1:value:int8" }), deviceapi::getPinValue );
    addDeviceCallback( "/device/eeprom/read",       URLParams_t({ "did", "address" }), URLParams_t({ "1:value:int8" }), deviceapi::readEEPROM );
    addDeviceCallback( "/device/eeprom/write",      URLParams_t({ "did", "address", "value" }), URLParams_t({ "1:savedvalue:int8" }), deviceapi::writeEEPROM );
    addDeviceCallback( "/device/localtime",         URLParams_t({ "did" }), URLParams_t({ "1:time:ulong" }), deviceapi::getLocalTime );
    addDeviceCallback( "/device/setlocaltime",      URLParams_t({ "did", "time" }), URLParams_t({ "1:localtime:ulong" }), deviceapi::setLocalTime );


}


